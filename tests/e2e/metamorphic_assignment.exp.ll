; ModuleID = 'tests/e2e/metamorphic_assignment.ll'
source_filename = "metamorphic_assignment.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@j = dso_local global i32 0, align 4
@i = dso_local global i32 0, align 4
@USEVAR = external global i32, align 4
@INPUT = external global i32, align 4
@__cos_ssa_current_context_0 = internal global i32 0

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @fn() #0 {
  %1 = load i32, ptr @__cos_ssa_current_context_0, align 4
  %2 = icmp eq i32 %1, 2
  br i1 %2, label %true, label %false

3:                                                ; preds = %false2, %true1, %true
  %4 = load i32, ptr @i, align 4
  store i32 %4, ptr @USEVAR, align 4
  ret void

true:                                             ; preds = %0
  store i32 20, ptr @i, align 4
  br label %3

false:                                            ; preds = %0
  %5 = icmp eq i32 %1, 3
  br i1 %5, label %true1, label %false2

true1:                                            ; preds = %false
  store i32 22, ptr @i, align 4
  br label %3

false2:                                           ; preds = %false
  store i32 24, ptr @i, align 4
  br label %3
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
  %1 = alloca i32, align 4
  store i32 0, ptr %1, align 4
  %2 = load i32, ptr @INPUT, align 4
  %3 = icmp eq i32 %2, 0
  br i1 %3, label %4, label %6

4:                                                ; preds = %0
  %5 = load i32, ptr @__cos_ssa_current_context_0, align 4
  store i32 2, ptr @__cos_ssa_current_context_0, align 4
  call void @fn()
  store i32 %5, ptr @__cos_ssa_current_context_0, align 4
  br label %14

6:                                                ; preds = %0
  %7 = load i32, ptr @INPUT, align 4
  %8 = icmp eq i32 %7, 1
  br i1 %8, label %9, label %11

9:                                                ; preds = %6
  %10 = load i32, ptr @__cos_ssa_current_context_0, align 4
  store i32 3, ptr @__cos_ssa_current_context_0, align 4
  call void @fn()
  store i32 %10, ptr @__cos_ssa_current_context_0, align 4
  br label %13

11:                                               ; preds = %6
  %12 = load i32, ptr @__cos_ssa_current_context_0, align 4
  store i32 4, ptr @__cos_ssa_current_context_0, align 4
  call void @fn()
  store i32 %12, ptr @__cos_ssa_current_context_0, align 4
  br label %13

13:                                               ; preds = %11, %9
  br label %14

14:                                               ; preds = %13, %4
  ret i32 0
}

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"Ubuntu clang version 15.0.7"}
