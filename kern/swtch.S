# Context switch
#
#   void swtch(struct context **old, struct context *new);
# 
# Save the current registers on the stack, creating
# a struct context, and save its address in *old.
# Switch stacks to new and pop previously-saved registers.

.globl swtch
swtch:
  movq %rdi, %rax
  movq %rsi, %rdx

  # Save old callee-saved registers
  pushq %rbp
  pushq %rbx
  pushq %rsi
  pushq %rdi
  pushq %r12
  pushq %r13
  pushq %r14
  pushq %r15

  # Switch stacks
  movq %rsp, (%rax)
  movq %rdx, %rsp

  # Load new callee-saved registers
  popq %r12
  popq %r13
  popq %r14
  popq %r15
  popq %rdi
  popq %rsi
  popq %rbx
  popq %rbp
  ret
