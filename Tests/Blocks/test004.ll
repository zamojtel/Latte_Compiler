define dso_local noundef i32 @main() {
%1 = alloca i32, align 4
%2 = alloca i32, align 4
%3 = alloca i32, align 4
%4 = alloca i32, align 4
%5 = alloca i32, align 4
%6 = alloca i32, align 4
store i32 0, i32* %1, align 4
store i32 0, i32* %2, align 4
store i32 0, i32* %3, align 4
store i32 10, i32* %1, align 4
store i32 20, i32* %2, align 4
store i32 30, i32* %3, align 4
br label %Label0
Label0:
%7 = load i32, i32* %1, align 4
%8 = icmp slt i32 %7, 100
br i1 %8, label %Label7, label %Label5
Label7:
%9 = load i32, i32* %1, align 4
%10 = icmp slt i32 %9, 10
br i1 %10, label %Label8, label %Label1
Label8:
%11 = load i32, i32* %2, align 4
%12 = load i32, i32* %3, align 4
%13 = add nsw i32 %11, %12
store i32 %13, i32* %4, align 4
%14 = load i32, i32* %1, align 4
%15 = load i32, i32* %4, align 4
%16 = add nsw i32 %14, %15
store i32 %16, i32* %1, align 4
br label %Label0
Label1:
%17 = load i32, i32* %2, align 4
%18 = load i32, i32* %3, align 4
%19 = icmp slt i32 %17, %18
br i1 %19, label %Label9, label %Label2
Label9:
%20 = load i32, i32* %3, align 4
%21 = add nsw i32 %20, -1
store i32 %21, i32* %3, align 4
br label %Label0
Label2:
%22 = load i32, i32* %2, align 4
%23 = add nsw i32 %22, 1
store i32 %23, i32* %2, align 4
br label %Label3
Label3:
%24 = load i32, i32* %1, align 4
%25 = add nsw i32 %24, 1
store i32 %25, i32* %1, align 4
br label %Label4
Label4:
%26 = load i32, i32* %1, align 4
%27 = load i32, i32* %2, align 4
%28 = add nsw i32 %26, %27
%29 = load i32, i32* %3, align 4
%30 = add nsw i32 %28, %29
store i32 %30, i32* %5, align 4
br label %Label0
Label5:
%31 = load i32, i32* %1, align 4
%32 = load i32, i32* %2, align 4
%33 = add nsw i32 %31, %32
%34 = load i32, i32* %3, align 4
%35 = add nsw i32 %33, %34
store i32 %35, i32* %6, align 4
ret i32 0
}
declare dso_local noundef i8* @addStrings(i8* noundef %0, i8* noundef %1)
declare dso_local void @printInt(i32 noundef %0)
declare dso_local void @printString(i8* noundef %0)
declare dso_local noundef i8* @readString()
declare dso_local noundef i32 @readInt()
declare dso_local void @increaseStringCounter(i8* noundef %0)
declare dso_local void @decreaseStringCounter(i8* noundef %0)
declare dso_local i32 @getArrayLength(i8* noundef %0)
declare dso_local i8* @allocArray(i32 noundef %0, i32 noundef %1)
declare dso_local zeroext i1 @isConstant(i8* noundef %0)
declare dso_local i32 @getReferenceCount(i8* noundef %0)
declare dso_local zeroext i1 @compareStrings(i8* noundef %0, i8* noundef %1)
declare dso_local i8* @allocateInstance(i8** noundef %0, i32 noundef %1)
declare dso_local i8* @getField(i8* noundef %0, i32 noundef %1)
declare dso_local i8** @get_vtable(i8* noundef %0)
