; ModuleID = 'main.bc'
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64-S128"
target triple = "x86_64-redhat-linux-gnu"

@a = global i32 0

define i32 @main(i32 %arg1, i32 %arg2, i32 %arg3) {
entry:
  %arg11 = alloca i32
  store i32 %arg1, i32* %arg11
  %arg22 = alloca i32
  store i32 %arg1, i32* %arg22
  %arg33 = alloca i32
  store i32 %arg1, i32* %arg33
  br label %body

body:                                             ; preds = %entry
  %x = alloca i32
  %v = alloca <4 x float>
  %arg24 = load i32* %arg22
  %0 = add i32 %arg24, 1
  store i32 %0, i32* %arg22
  %arg15 = load i32* %arg11
  %1 = add i32 %arg15, %0
  store i32 %1, i32* %x
  %2 = icmp oeq i1 false, false
  br i1 %2, label %then, label %Foot

Foot:                                             ; preds = %Foot, %body
  %arg16 = load i32* %arg11
  %3 = sub i32 %arg16, 1
  store i32 %3, i32* %arg11
  %x7 = load i32* %x
  %4 = sub i32 %x7, %3
  store i32 %4, i32* %x
  br label %Foot

then:                                             ; preds = %body
  %arg38 = load i32* %arg33
  %a = load i32* @a
  %5 = add i32 %a, %arg38
  %6 = mul i32 2, %5
  %x9 = load i32* %x
  %7 = add i32 %x9, %6
  ret i32 %7
}
