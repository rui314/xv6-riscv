// which hart (core) is this?
extern uint64 r_mhartid();

// Machine Status Register, mstatus

#define MSTATUS_MPP_MASK (3L << 11) // previous mode.
#define MSTATUS_MPP_M (3L << 11)
#define MSTATUS_MPP_S (1L << 11)
#define MSTATUS_MPP_U (0L << 11)
#define MSTATUS_MIE (1L << 3)    // machine-mode interrupt enable.

extern uint64 r_mstatus();
extern void w_mstatus(uint64 x);

// machine exception program counter, holds the
// instruction address to which a return from
// exception will go.
void w_mepc(uint64 x);

// Supervisor Status Register, sstatus

#define SSTATUS_SPP (1L << 8)  // Previous mode, 1=Supervisor, 0=User
#define SSTATUS_SPIE (1L << 5) // Supervisor Previous Interrupt Enable
#define SSTATUS_UPIE (1L << 4) // User Previous Interrupt Enable
#define SSTATUS_SIE (1L << 1)  // Supervisor Interrupt Enable
#define SSTATUS_UIE (1L << 0)  // User Interrupt Enable

extern uint64 r_sstatus();
extern void w_sstatus(uint64 x);

// Supervisor Interrupt Pending
extern uint64 r_sip();

void w_sip(uint64 x);

// Supervisor Interrupt Enable
#define SIE_SEIE (1L << 9) // external
#define SIE_STIE (1L << 5) // timer
#define SIE_SSIE (1L << 1) // software

extern uint64 r_sie();

extern void w_sie(uint64 x);

// Machine-mode Interrupt Enable
#define MIE_MEIE (1L << 11) // external
#define MIE_MTIE (1L << 7)  // timer
#define MIE_MSIE (1L << 3)  // software

extern uint64 r_mie();

extern void w_mie(uint64 x);

// machine exception program counter, holds the
// instruction address to which a return from
// exception will go.
extern void w_sepc(uint64 x);

extern uint64 r_sepc();

// Machine Exception Delegation
extern uint64 r_medeleg();

extern void w_medeleg(uint64 x);

// Machine Interrupt Delegation
extern uint64 r_mideleg();

extern void w_mideleg(uint64 x);

// Supervisor Trap-Vector Base Address
// low two bits are mode.
extern void w_stvec(uint64 x);

extern uint64 r_stvec();

// Machine-mode interrupt vector
extern void w_mtvec(uint64 x);

// use riscv's sv39 page table scheme.
#define SATP_SV39 (8L << 60)

#define MAKE_SATP(pagetable) (SATP_SV39 | (((uint64)pagetable) >> 12))

// supervisor address translation and protection;
// holds the address of the page table.
extern void w_satp(uint64 x);

extern uint64 r_satp();

// Supervisor Scratch register, for early trap handler in trampoline.S.
extern void w_sscratch(uint64 x);

extern void w_mscratch(uint64 x);

// Supervisor Trap Cause
extern uint64 r_scause();

// Supervisor Trap Value
extern uint64 r_stval();

// Machine-mode Counter-Enable
extern void w_mcounteren(uint64 x);

extern uint64 r_mcounteren();

// machine-mode cycle counter
extern uint64 r_time();

// enable device interrupts
static inline void
intr_on()
{
  w_sie(r_sie() | SIE_SEIE | SIE_STIE | SIE_SSIE);
  w_sstatus(r_sstatus() | SSTATUS_SIE);
}

// disable device interrupts
static inline void
intr_off()
{
  w_sstatus(r_sstatus() & ~SSTATUS_SIE);
}

// are device interrupts enabled?
static inline int
intr_get()
{
  uint64 x = r_sstatus();
  return (x & SSTATUS_SIE) != 0;
}

// read and write tp, the thread pointer, which holds
// this core's hartid (core number), the index into cpus[].
extern uint64 r_tp();
extern void w_tp(uint64 x);

// flush the TLB.
extern void sfence_vma();

#define PGSIZE 4096 // bytes per page
#define PGSHIFT 12  // bits of offset within a page

#define PGROUNDUP(sz)  (((sz)+PGSIZE-1) & ~(PGSIZE-1))
#define PGROUNDDOWN(a) (((a)) & ~(PGSIZE-1))

#define PTE_V (1L << 0) // valid
#define PTE_R (1L << 1)
#define PTE_W (1L << 2)
#define PTE_X (1L << 3)
#define PTE_U (1L << 4) // 1 -> user can access

// shift a physical address to the right place for a PTE.
#define PA2PTE(pa) ((((uint64)pa) >> 12) << 10)

#define PTE2PA(pte) (((pte) >> 10) << 12)

#define PTE_FLAGS(pte) ((pte) & 0x3FF)

// extract the three 9-bit page table indices from a virtual address.
#define PXMASK          0x1FF // 9 bits
#define PXSHIFT(level)  (PGSHIFT+(9*(level)))
#define PX(level, va) ((((uint64) (va)) >> PXSHIFT(level)) & PXMASK)

// one beyond the highest possible virtual address.
// MAXVA is actually one bit less than the max allowed by
// Sv39, to avoid having to sign-extend virtual addresses
// that have the high bit set.
#define MAXVA (1L << (9 + 9 + 9 + 12 - 1))

typedef uint64 pte_t;
typedef uint64 *pagetable_t; // 512 PTEs
