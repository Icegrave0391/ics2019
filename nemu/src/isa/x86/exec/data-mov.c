#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push) {
	//TODO();
	/*
	(* StackAddrSize = 32 *)
   IF OperandSize = 16
   THEN
      ESP := ESP - 2;
      (SS:ESP) := (SOURCE); (* word assignment *)
   ELSE
      ESP := ESP - 4;
      (SS:ESP) := (SOURCE); (* dword assignment *)
   FI;
	FI;
	*/
	rtl_push(&id_dest->val);
	print_asm_template1(push);
}

make_EHelper(pop) {
  //TODO();
	/*
	(* StackAddrSize = 32 * )
   IF OperandSize = 16
   THEN
      DEST := (SS:ESP); (* copy a word *)
      ESP := ESP + 2;
   ELSE (* OperandSize = 32 *)
      DEST := (SS:ESP); (* copy a dword *)
      ESP := ESP + 4;
   FI;
	FI;
	*/
	rtl_pop(&s0);
	id_dest->val = s0;
	operand_write(id_dest, &s0);
	print_asm_template1(pop);
}

make_EHelper(pusha) {
  // TODO();
	/*
	 Temp := (ESP);
   Push(EAX);
   Push(ECX);
   Push(EDX);
   Push(EBX);
   Push(Temp);
   Push(EBP);
   Push(ESI);
   Push(EDI);
	*/
	s0 = reg_l(R_ESP);
	rtl_push(&reg_l(R_EAX));
	rtl_push(&reg_l(R_ECX));
	rtl_push(&reg_l(R_EDX));
	rtl_push(&reg_l(R_EBX));
	rtl_push(&reg_l(s0));
	rtl_push(&reg_l(R_EBP));
	rtl_push(&reg_l(R_ESI));
	rtl_push(&reg_l(R_EDI));
	print_asm("pusha");
}

make_EHelper(popa) {
  // TODO();
	rtl_pop(&reg_l(R_EDI));
	rtl_pop(&reg_l(R_ESI));
	rtl_pop(&reg_l(R_EBP));
	reg_l(R_ESP) += 4; //  Skip esp
	rtl_pop(&reg_l(R_EBX));
	rtl_pop(&reg_l(R_EDX));
	rtl_pop(&reg_l(R_ECX));
	rtl_pop(&reg_l(R_EAX));
	print_asm("popa");
}

make_EHelper(leave) {
  TODO();

  print_asm("leave");
}

make_EHelper(cltd) {
  if (decinfo.isa.is_operand_size_16) {
    TODO();
  }
  else {
    TODO();
  }

  print_asm(decinfo.isa.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl) {
  if (decinfo.isa.is_operand_size_16) {
    TODO();
  }
  else {
    TODO();
  }

  print_asm(decinfo.isa.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx) {
  id_dest->width = decinfo.isa.is_operand_size_16 ? 2 : 4;
  rtl_sext(&s0, &id_src->val, id_src->width);
  operand_write(id_dest, &s0);
  print_asm_template2(movsx);
}

make_EHelper(movzx) {
  id_dest->width = decinfo.isa.is_operand_size_16 ? 2 : 4;
  operand_write(id_dest, &id_src->val);
  print_asm_template2(movzx);
}

make_EHelper(lea) {
  operand_write(id_dest, &id_src->addr);
  print_asm_template2(lea);
}
