; LLVM IR Generated from Custom Compiler
; Target: x86_64-pc-windows-msvc

declare i32 @printf(i8*, ...)
@.str = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

define i32 @add() {
entry:
  %t1 = add i32 %a, %b
  %result = alloca i32, align 4
  store i32 %t1, i32* %result, align 4
  ret i32 %result
}

define i32 @factorial() {
entry:
  %i = alloca i32, align 4
  store i32 1, i32* %i, align 4
  %f = alloca i32, align 4
  store i32 1, i32* %f, align 4
L_while_start_1:
  %t2 = icmp slt i32 %i, %n
  %1 = zext i1 %t2 to i32
  %2 = icmp ne i32 %1, 0
  br i1 %2, label %L_while_body_2, label %next_2
next_2:
  br label %L_while_end_3
L_while_body_2:
  %t3 = mul i32 %f, %i
  %f = alloca i32, align 4
  store i32 %t3, i32* %f, align 4
  %t4 = add i32 %i, 1
  %i = alloca i32, align 4
  store i32 %t4, i32* %i, align 4
  br label %L_while_start_1
L_while_end_3:
  ret i32 %f
}

define i32 @max3() {
entry:
  %m = alloca i32, align 4
  store i32 %a, i32* %m, align 4
  %t5 = icmp sgt i32 %b, %m
  %1 = zext i1 %t5 to i32
  %2 = icmp ne i32 %1, 0
  br i1 %2, label %L_if_then_4, label %next_2
next_2:
  br label %L_if_else_5
L_if_then_4:
  %m = alloca i32, align 4
  store i32 %b, i32* %m, align 4
  br label %L_if_end_6
L_if_else_5:
L_if_end_6:
  %t6 = icmp sgt i32 %c, %m
  %3 = zext i1 %t6 to i32
  %4 = icmp ne i32 %3, 0
  br i1 %4, label %L_if_then_7, label %next_4
next_4:
  br label %L_if_else_8
L_if_then_7:
  %m = alloca i32, align 4
  store i32 %c, i32* %m, align 4
  br label %L_if_end_9
L_if_else_8:
L_if_end_9:
  ret i32 %m
}

define i32 @computeNested() {
entry:
  %t7 = add i32 %x, 10
  %y = alloca i32, align 4
  store i32 %t7, i32* %y, align 4
  %t8 = mul i32 %y, 2
  %z = alloca i32, align 4
  store i32 %t8, i32* %z, align 4
  %t9 = icmp sgt i32 %z, 50
  %1 = zext i1 %t9 to i32
  %2 = icmp ne i32 %1, 0
  br i1 %2, label %L_if_then_10, label %next_2
next_2:
  br label %L_if_else_11
L_if_then_10:
  %t10 = sub i32 %z, 5
  %z = alloca i32, align 4
  store i32 %t10, i32* %z, align 4
  %t11 = icmp sgt i32 %z, 80
  %3 = zext i1 %t11 to i32
  %4 = icmp ne i32 %3, 0
  br i1 %4, label %L_if_then_13, label %next_4
next_4:
  br label %L_if_else_14
L_if_then_13:
  %t12 = sdiv i32 %z, 2
  %z = alloca i32, align 4
  store i32 %t12, i32* %z, align 4
  br label %L_if_end_15
L_if_else_14:
L_if_end_15:
  br label %L_if_end_12
L_if_else_11:
  %t13 = add i32 %z, 100
  %z = alloca i32, align 4
  store i32 %t13, i32* %z, align 4
L_if_end_12:
  ret i32 %z
}

define i32 @main() {
entry:
  %s = alloca i32, align 4
  store i32 "compiler_test", i32* %s, align 4
  %total = alloca i32, align 4
  store i32 0, i32* %total, align 4
  %i = alloca i32, align 4
  store i32 0, i32* %i, align 4
L_for_start_16:
  %t14 = icmp slt i32 %i, 5
  %1 = zext i1 %t14 to i32
  %2 = icmp ne i32 %1, 0
  br i1 %2, label %L_for_body_17, label %next_2
next_2:
  br label %L_for_end_18
L_for_body_17:
  %t15 = add i32 %total, %i
  %total = alloca i32, align 4
  store i32 %t15, i32* %total, align 4
  %t16 = add i32 %i, 1
  %i = alloca i32, align 4
  store i32 %t16, i32* %i, align 4
  br label %L_for_start_16
L_for_end_18:
  %sum2 = alloca i32, align 4
  store i32 0, i32* %sum2, align 4
  %x = alloca i32, align 4
  store i32 0, i32* %x, align 4
L_while_start_19:
  %t17 = icmp slt i32 %x, 3
  %3 = zext i1 %t17 to i32
  %4 = icmp ne i32 %3, 0
  br i1 %4, label %L_while_body_20, label %next_4
next_4:
  br label %L_while_end_21
L_while_body_20:
  %y = alloca i32, align 4
  store i32 0, i32* %y, align 4
L_while_start_22:
  %t18 = icmp slt i32 %y, 3
  %5 = zext i1 %t18 to i32
  %6 = icmp ne i32 %5, 0
  br i1 %6, label %L_while_body_23, label %next_6
next_6:
  br label %L_while_end_24
L_while_body_23:
  %t19 = mul i32 %x, %y
  %t20 = add i32 %sum2, %t19
  %sum2 = alloca i32, align 4
  store i32 %t20, i32* %sum2, align 4
  %t21 = add i32 %y, 1
  %y = alloca i32, align 4
  store i32 %t21, i32* %y, align 4
  br label %L_while_start_22
L_while_end_24:
  %t22 = add i32 %x, 1
  %x = alloca i32, align 4
  store i32 %t22, i32* %x, align 4
  br label %L_while_start_19
L_while_end_21:
  %t23 = call i32 @max3()
  %maximum = alloca i32, align 4
  store i32 %t23, i32* %maximum, align 4
  %j = alloca i32, align 4
  store i32 0, i32* %j, align 4
L_for_start_25:
  %t24 = icmp slt i32 %j, 20
  %7 = zext i1 %t24 to i32
  %8 = icmp ne i32 %7, 0
  br i1 %8, label %L_for_body_26, label %next_8
next_8:
  br label %L_for_end_27
L_for_body_26:
  %t25 = sdiv i32 %j, 4
  %t26 = icmp eq i32 %t25, 0
  %9 = zext i1 %t26 to i32
  %10 = icmp ne i32 %9, 0
  br i1 %10, label %L_if_then_28, label %next_10
next_10:
  br label %L_if_else_29
L_if_then_28:
  %t27 = add i32 %total, %j
  %total = alloca i32, align 4
  store i32 %t27, i32* %total, align 4
  br label %L_if_end_30
L_if_else_29:
L_if_end_30:
  %t28 = add i32 %j, 2
  %j = alloca i32, align 4
  store i32 %t28, i32* %j, align 4
  br label %L_for_start_25
L_for_end_27:
  %t29 = call i32 @factorial()
  %f5 = alloca i32, align 4
  store i32 %t29, i32* %f5, align 4
  %t30 = call i32 @add()
  %sum_ab = alloca i32, align 4
  store i32 %t30, i32* %sum_ab, align 4
  %t31 = call i32 @computeNested()
  %nestedResult = alloca i32, align 4
  store i32 %t31, i32* %nestedResult, align 4
  %t32 = add i32 %total, %sum2
  %t33 = add i32 %t32, %maximum
  %t34 = add i32 %t33, %f5
  %t35 = add i32 %t34, %sum_ab
  %t36 = add i32 %t35, %nestedResult
  ret i32 %t36
  ret i32 0
}

