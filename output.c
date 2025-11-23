#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Forward declarations
int add(int a, int b);
int factorial(int n);
int max3(int a, int b, int c);
int computeNested(int x);

int add(int a, int b) {
int result;
int t1;

    t1 = a + b;
    result = t1;
    return result;
}

int factorial(int n) {
int f;
int i;
int t2;
int t3;
int t4;

    i = 1;
    f = 1;
L_while_start_1:
    t2 = i < n;
    if (t2) goto L_while_body_2;
    goto L_while_end_3;
L_while_body_2:
    t3 = f * i;
    f = t3;
    t4 = i + 1;
    i = t4;
    goto L_while_start_1;
L_while_end_3:
    return f;
}

int max3(int a, int b, int c) {
int m;
int t5;
int t6;

    m = a;
    t5 = b > m;
    if (t5) goto L_if_then_4;
    goto L_if_else_5;
L_if_then_4:
    m = b;
    goto L_if_end_6;
L_if_else_5:
L_if_end_6:
    t6 = c > m;
    if (t6) goto L_if_then_7;
    goto L_if_else_8;
L_if_then_7:
    m = c;
    goto L_if_end_9;
L_if_else_8:
L_if_end_9:
    return m;
}

int computeNested(int x) {
int t10;
int t11;
int t12;
int t13;
int t7;
int t8;
int t9;
int y;
int z;

    t7 = x + 10;
    y = t7;
    t8 = y * 2;
    z = t8;
    t9 = z > 50;
    if (t9) goto L_if_then_10;
    goto L_if_else_11;
L_if_then_10:
    t10 = z - 5;
    z = t10;
    t11 = z > 80;
    if (t11) goto L_if_then_13;
    goto L_if_else_14;
L_if_then_13:
    t12 = z / 2;
    z = t12;
    goto L_if_end_15;
L_if_else_14:
L_if_end_15:
    goto L_if_end_12;
L_if_else_11:
    t13 = z + 100;
    z = t13;
L_if_end_12:
    return z;
}

int main() {
int f5;
int i;
int j;
int maximum;
int nestedResult;
char* s;
int sum2;
int sum_ab;
int t14;
int t15;
int t16;
int t17;
int t18;
int t19;
int t20;
int t21;
int t22;
int t23;
int t24;
int t25;
int t26;
int t27;
int t28;
int t29;
int t30;
int t31;
int t32;
int t33;
int t34;
int t35;
int t36;
int total;
int x;
int y;

    s = "compiler_test";
    total = 0;
    i = 0;
L_for_start_16:
    t14 = i < 5;
    if (t14) goto L_for_body_17;
    goto L_for_end_18;
L_for_body_17:
    t15 = total + i;
    total = t15;
    t16 = i + 1;
    i = t16;
    goto L_for_start_16;
L_for_end_18:
    sum2 = 0;
    x = 0;
L_while_start_19:
    t17 = x < 3;
    if (t17) goto L_while_body_20;
    goto L_while_end_21;
L_while_body_20:
    y = 0;
L_while_start_22:
    t18 = y < 3;
    if (t18) goto L_while_body_23;
    goto L_while_end_24;
L_while_body_23:
    t19 = x * y;
    t20 = sum2 + t19;
    sum2 = t20;
    t21 = y + 1;
    y = t21;
    goto L_while_start_22;
L_while_end_24:
    t22 = x + 1;
    x = t22;
    goto L_while_start_19;
L_while_end_21:
    t23 = max3(10, 40, 25);
    maximum = t23;
    j = 0;
L_for_start_25:
    t24 = j < 20;
    if (t24) goto L_for_body_26;
    goto L_for_end_27;
L_for_body_26:
    t25 = j / 4;
    t26 = t25 == 0;
    if (t26) goto L_if_then_28;
    goto L_if_else_29;
L_if_then_28:
    t27 = total + j;
    total = t27;
    goto L_if_end_30;
L_if_else_29:
L_if_end_30:
    t28 = j + 2;
    j = t28;
    goto L_for_start_25;
L_for_end_27:
    t29 = factorial(5);
    f5 = t29;
    t30 = add(12, 30);
    sum_ab = t30;
    t31 = computeNested(21);
    nestedResult = t31;
    t32 = total + sum2;
    t33 = t32 + maximum;
    t34 = t33 + f5;
    t35 = t34 + sum_ab;
    t36 = t35 + nestedResult;
    return t36;
    return 0;
}

