#include "csc_can.h"
#include "string.h"

#include "LPC21xx.h"
#include "armVIC.h"

#include "led.h"


#ifdef USE_CAN1

#define QUEUE_LEN 8

struct MsgQueue {
  int head;
  int tail;
  int full;
  struct CscCanMsg msgs[QUEUE_LEN];
};

static struct MsgQueue can1_tx_queue;
static struct MsgQueue can1_rx_queue;

bool_t can1_msg_received;
struct CscCanMsg can1_rx_msg;
static void (* can1_callback)(struct CscCanMsg *);

static void CAN1_Rx_ISR ( void ) __attribute__((naked));
/*static void CAN1_Tx_ISR ( void ) __attribute__((naked));*/
static void CAN1_Err_ISR ( void ) __attribute__((naked));

static void msg_queue_init(struct MsgQueue *q)
{
  q->head = 0;
  q->tail = 0;
  q->full = 0;
}

static int msg_queue_full(struct MsgQueue *q)
{
  return q->full;
}

static int msg_queue_empty(struct MsgQueue *q)
{
  return (q->head == q->tail) && !q->full;
}

static void msg_enqueue(struct MsgQueue *q, struct CscCanMsg *msg)
{
  memcpy(&q->msgs[q->tail], msg, sizeof (struct CscCanMsg));
  q->tail = (q->tail + 1) % QUEUE_LEN;
  if (q->head == q->tail) q->full = 1;
}

static void msg_dequeue(struct MsgQueue *q, struct CscCanMsg *msg)
{
  memcpy(msg, &q->msgs[q->head], sizeof(struct CscCanMsg));
  q->head = (q->head + 1) % QUEUE_LEN;
  q->full = 0;
}


static void can1_hw_init(void)
{
  // Acceptance Filter Mode Register = filter off, receive all
  AFMR = 0x00000002;

  // Go into Reset mode
  C1MOD =  1;
  // Clear Status register (including error counters)
  C1GSR = 0;

  // Set bit timing
  C1BTR = CAN1_BTR;

  // Disable All Interrupts
  C1IER = 0;

  // Enable error interrupts if handled
#ifdef  CAN1_ERR_VIC_SLOT
  C1IER =  (1<<0) | /* RIE */
           (1<<2) | /* EIE */
           (1<<6) | /* ALIE */
           (1<<7) | /* BEIE */
           (1<<7)   /* BEIE */
    ;
#endif

  // Get out of reset Mode
  C1MOD = 0;
}

void csc_can1_init(void(* callback)(struct CscCanMsg *msg))
{

  // Initialize the interrupt vector
  VICIntSelect &= ~VIC_BIT(VIC_CAN1_RX);               // VIC_CAN1_RX selected as IRQ
  VICIntEnable = VIC_BIT(VIC_CAN1_RX);                 // VIC_CAN1_RX interrupt enabled
  _VIC_CNTL(CAN1_VIC_SLOT) = VIC_ENABLE | VIC_CAN1_RX; //
  _VIC_ADDR(CAN1_VIC_SLOT) = (uint32_t)CAN1_Rx_ISR;    // address of the ISR

  // intitialze error interrupt
#ifdef  CAN1_ERR_VIC_SLOT
  VICIntSelect &= ~VIC_BIT(VIC_CAN);                  // VIC_CAN selected as IRQ
  VICIntEnable = VIC_BIT(VIC_CAN);                    // VIC_CAN interrupt enabled
  _VIC_CNTL(CAN1_ERR_VIC_SLOT) = VIC_ENABLE | VIC_CAN; //
  _VIC_ADDR(CAN1_ERR_VIC_SLOT) = (uint32_t)CAN1_Err_ISR;
#endif

  // Set bit 18
  PINSEL1 |= _BV(18);

  // set can callback before enabling interrupts
  can1_callback = callback;

  msg_queue_init(&can1_tx_queue);
  msg_queue_init(&can1_rx_queue);

  // initialize actual hardware
  can1_hw_init();
}

static inline uint32_t csc_can1_tx_available()
{
  return (C1SR & 0x00000004L);
}


static inline int can1_bus_off()
{
  // check for bit seven of CAN1 GSR (Bus-off state)
  return C1SR & _BV(7);
}

void csc_can1_send(struct CscCanMsg* msg) {

  if (can1_bus_off()) {
    can1_hw_init();
  }

  if (!csc_can1_tx_available()) { /* transmit channel not available */
    if (! msg_queue_full(&can1_tx_queue)) {
      msg_enqueue(&can1_tx_queue, msg);
    }

    //    LED_ON(2);
    return;
  }
  //  LED_OFF(2);

  // Write DLC, RTR and FF
  C1TFI1 = (msg->frame &  0xC00F0000L);
  // Write CAN ID
  C1TID1 = msg->id;
  // Write first 4 data bytes
  C1TDA1 = msg->dat_a;
  // Write second 4 data bytes
  C1TDB1 = msg->dat_b;
  // Write self reception request
  //  C1CMR = 0x30;
  // write transmission request
  C1CMR = 0x21;

}

void CAN1_Rx_ISR ( void ) {
 ISR_ENTRY();

 can1_rx_msg.id = C1RID;
 if (BOARDID_OF_CANMSG_ID(can1_rx_msg.id) == CSC_BOARD_ID) {
   can1_rx_msg.frame  = C1RFS;
   can1_rx_msg.dat_a  = C1RDA;
   can1_rx_msg.dat_b  = C1RDB;
   can1_msg_received = TRUE;
    if (! msg_queue_full(&can1_rx_queue)) {
      msg_enqueue(&can1_rx_queue, &can1_rx_msg);
   }
  }


 C1CMR = 0x04;             // release receive buffer
 VICVectAddr = 0x00000000; // acknowledge interrupt

 ISR_EXIT();
}


/*
void CAN1_Tx_ISR ( void ) {
 ISR_ENTRY();

 ISR_EXIT();
}
*/

#endif /* USE_CAN1 */

void csc_can_event(void)
{
#ifdef USE_CAN1

  // drain the RX Queue
  while(!msg_queue_empty(&can1_rx_queue)) {
    struct CscCanMsg rx_msg;
    //cpsr = disableIRQ();                  // disable global interrupts
    msg_dequeue(&can1_rx_queue, &rx_msg);
    can1_callback(&rx_msg);
    //restoreIRQ(cpsr);                     // restore global interrupts
  }

  if(!msg_queue_empty(&can1_tx_queue) && csc_can1_tx_available()) {
    struct CscCanMsg msg;
    msg_dequeue(&can1_tx_queue, &msg);
    csc_can1_send(&msg);
  }

#endif /* USE_CAN1 */
}

static uint32_t err_cnt = 0;

void CAN1_Err_ISR ( void ) {
 ISR_ENTRY();
 uint32_t __attribute__ ((unused)) c1icr = C1ICR;

 err_cnt++;
 /* LED_ON(ERROR_LED); */

 VICVectAddr = 0x00000000; // acknowledge interrupt
 ISR_EXIT();
}
