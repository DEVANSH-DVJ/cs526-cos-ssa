; ModuleID = 'tests/e2e/multiple_partitions.ll'
source_filename = "multiple_partitions.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@i = dso_local global i32 0, align 4
@USEVAR = external global i32, align 4
@j = dso_local global i32 0, align 4
@INPUT1 = external global i32, align 4
@INPUT2 = external global i32, align 4
@__cos_ssa_current_context_0 = internal global i32 0
@__cos_ssa_current_context_1 = internal global i32 0

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @fn1() #0 {
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
  store i32 30, ptr @i, align 4
  br label %3

false2:                                           ; preds = %false
  %6 = load i32, ptr @i, align 4
  %7 = add i32 %6, 20
  store i32 %7, ptr @i, align 4
  br label %3
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @fn2() #0 {
  %1 = load i32, ptr @__cos_ssa_current_context_0, align 4
  %2 = icmp eq i32 %1, 4
  br i1 %2, label %true, label %false

3:                                                ; preds = %false2, %true1, %true
  %4 = load i32, ptr @i, align 4
  store i32 %4, ptr @USEVAR, align 4
  ret void

true:                                             ; preds = %0
  store i32 40, ptr @i, align 4
  br label %3

false:                                            ; preds = %0
  %5 = icmp eq i32 %1, 5
  br i1 %5, label %true1, label %false2

true1:                                            ; preds = %false
  store i32 50, ptr @i, align 4
  br label %3

false2:                                           ; preds = %false
  %6 = load i32, ptr @i, align 4
  %7 = add i32 %6, 20
  store i32 %7, ptr @i, align 4
  br label %3
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
  %1 = alloca i32, align 4
  store i32 0, ptr %1, align 4
  %2 = load i32, ptr @INPUT1, align 4
  %3 = icmp eq i32 %2, 0
  br i1 %3, label %4, label %9

4:                                                ; preds = %0
  %5 = load i32, ptr @__cos_ssa_current_context_0, align 4
  store i32 2, ptr @__cos_ssa_current_context_0, align 4
  %6 = load i32, ptr @__cos_ssa_current_context_1, align 4
  store i32 2, ptr @__cos_ssa_current_context_1, align 4
  call void @fn1()
  store i32 %6, ptr @__cos_ssa_current_context_1, align 4
  store i32 %5, ptr @__cos_ssa_current_context_0, align 4
  %7 = load i32, ptr @__cos_ssa_current_context_0, align 4
  store i32 4, ptr @__cos_ssa_current_context_0, align 4
  %8 = load i32, ptr @__cos_ssa_current_context_1, align 4
  store i32 3, ptr @__cos_ssa_current_context_1, align 4
  call void @fn2()
  store i32 %8, ptr @__cos_ssa_current_context_1, align 4
  store i32 %7, ptr @__cos_ssa_current_context_0, align 4
  br label %14

9:                                                ; preds = %0
  %10 = load i32, ptr @__cos_ssa_current_context_0, align 4
  store i32 3, ptr @__cos_ssa_current_context_0, align 4
  %11 = load i32, ptr @__cos_ssa_current_context_1, align 4
  store i32 2, ptr @__cos_ssa_current_context_1, align 4
  call void @fn1()
  store i32 %11, ptr @__cos_ssa_current_context_1, align 4
  store i32 %10, ptr @__cos_ssa_current_context_0, align 4
  %12 = load i32, ptr @__cos_ssa_current_context_0, align 4
  store i32 5, ptr @__cos_ssa_current_context_0, align 4
  %13 = load i32, ptr @__cos_ssa_current_context_1, align 4
  store i32 3, ptr @__cos_ssa_current_context_1, align 4
  call void @fn2()
  store i32 %13, ptr @__cos_ssa_current_context_1, align 4
  store i32 %12, ptr @__cos_ssa_current_context_0, align 4
  br label %14

14:                                               ; preds = %9, %4
  %15 = load i32, ptr @INPUT2, align 4
  %16 = icmp eq i32 %15, 0
  br i1 %16, label %17, label %22

17:                                               ; preds = %14
  %18 = load i32, ptr @__cos_ssa_current_context_0, align 4
  store i32 6, ptr @__cos_ssa_current_context_0, align 4
  %19 = load i32, ptr @__cos_ssa_current_context_1, align 4
  store i32 4, ptr @__cos_ssa_current_context_1, align 4
  call void @fn1()
  store i32 %19, ptr @__cos_ssa_current_context_1, align 4
  store i32 %18, ptr @__cos_ssa_current_context_0, align 4
  %20 = load i32, ptr @__cos_ssa_current_context_0, align 4
  store i32 7, ptr @__cos_ssa_current_context_0, align 4
  %21 = load i32, ptr @__cos_ssa_current_context_1, align 4
  store i32 6, ptr @__cos_ssa_current_context_1, align 4
  call void @fn2()
  store i32 %21, ptr @__cos_ssa_current_context_1, align 4
  store i32 %20, ptr @__cos_ssa_current_context_0, align 4
  br label %27

22:                                               ; preds = %14
  %23 = load i32, ptr @__cos_ssa_current_context_0, align 4
  store i32 6, ptr @__cos_ssa_current_context_0, align 4
  %24 = load i32, ptr @__cos_ssa_current_context_1, align 4
  store i32 5, ptr @__cos_ssa_current_context_1, align 4
  call void @fn1()
  store i32 %24, ptr @__cos_ssa_current_context_1, align 4
  store i32 %23, ptr @__cos_ssa_current_context_0, align 4
  %25 = load i32, ptr @__cos_ssa_current_context_0, align 4
  store i32 7, ptr @__cos_ssa_current_context_0, align 4
  %26 = load i32, ptr @__cos_ssa_current_context_1, align 4
  store i32 7, ptr @__cos_ssa_current_context_1, align 4
  call void @fn2()
  store i32 %26, ptr @__cos_ssa_current_context_1, align 4
  store i32 %25, ptr @__cos_ssa_current_context_0, align 4
  br label %27

27:                                               ; preds = %22, %17
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
