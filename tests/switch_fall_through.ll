; ModuleID = 'switch_fall_through.bc'
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64-S128"
target triple = "x86_64-redhat-linux-gnu"

@v = global <2 x float> zeroinitializer
@a = global i32 0

define float @switchtest2() {
entry:
  br label %body

body:                                             ; preds = %entry
  %f = alloca float
  store float 0.000000e+00, float* %f
  %f1 = load float* %f
  ret float %f1
}
