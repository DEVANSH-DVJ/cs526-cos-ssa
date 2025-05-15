; ModuleID = 'metamorphic_assignment.c'
source_filename = "metamorphic_assignment.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@j = dso_local global i32 0, align 4
@i = dso_local global i32 0, align 4
@USEVAR = external global i32, align 4
@INPUT = external global i32, align 4

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @fn() #0 {
  %1 = load i32, ptr @j, align 4
  store i32 %1, ptr @i, align 4
  %2 = load i32, ptr @i, align 4
  store i32 %2, ptr @USEVAR, align 4
  ret void
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
  %1 = alloca i32, align 4
  store i32 0, ptr %1, align 4
  %2 = load i32, ptr @INPUT, align 4
  %3 = icmp eq i32 %2, 0
  br i1 %3, label %4, label %5

4:                                                ; preds = %0
  store i32 20, ptr @j, align 4
  call void @fn()
  br label %11

5:                                                ; preds = %0
  %6 = load i32, ptr @INPUT, align 4
  %7 = icmp eq i32 %6, 1
  br i1 %7, label %8, label %9

8:                                                ; preds = %5
  store i32 22, ptr @j, align 4
  call void @fn()
  br label %10

9:                                                ; preds = %5
  store i32 24, ptr @j, align 4
  call void @fn()
  br label %10

10:                                               ; preds = %9, %8
  br label %11

11:                                               ; preds = %10, %4
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
