define dso_local noundef i32 @main() {
%1 = alloca i32, align 4
%2 = alloca i32, align 4
store i32 41, i32* %1, align 4
%3 = load i32, i32* %1, align 4
%4 = add nsw i32 %3, 1
store i32 %4, i32* %2, align 4
call void @f(i32 noundef 41,i32 noundef 1)
ret i32 0
}
define dso_local void @f(i32 noundef %0, i32 noundef %1) {
%3 = alloca i32, align 4
%4 = alloca i32, align 4
store i32 %0, i32* %3, align 4
store i32 %1, i32* %4, align 4
%5 = load i32, i32* %4, align 4
%6 = load i32, i32* %4, align 4
%7 = mul nsw i32 %5, %6
%8 = load i32, i32* %3, align 4
%9 = add nsw i32 %8, %7
call void @printInt(i32 noundef %9)
%10 = load i32, i32* %3, align 4
%11 = load i32, i32* %4, align 4
%12 = icmp sgt i32 %10, %11
br i1 %12, label %Label3, label %Label0
Label3:
%13 = load i32, i32* %4, align 4
%14 = load i32, i32* %4, align 4
%15 = mul nsw i32 %13, %14
%16 = load i32, i32* %3, align 4
%17 = add nsw i32 %16, %15
call void @printInt(i32 noundef %17)
br label %Label1
Label0:
%18 = load i32, i32* %4, align 4
%19 = load i32, i32* %4, align 4
%20 = mul nsw i32 %18, %19
%21 = load i32, i32* %3, align 4
%22 = add nsw i32 %21, %20
call void @printInt(i32 noundef %22)
br label %Label1
Label1:
%23 = load i32, i32* %4, align 4
%24 = add nsw i32 %23, 1
store i32 %24, i32* %4, align 4
%25 = load i32, i32* %4, align 4
%26 = load i32, i32* %4, align 4
%27 = mul nsw i32 %25, %26
%28 = load i32, i32* %3, align 4
%29 = add nsw i32 %28, %27
call void @printInt(i32 noundef %29)
ret void
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
