@.str.1 = private unnamed_addr constant [17 x i8] c"\00\00\00\00\00Fac is even\00", align 1
@.str.2 = private unnamed_addr constant [16 x i8] c"\00\00\00\00\00Fac is odd\00", align 1
define dso_local noundef i32 @fac(i32 noundef %0) {
%2 = alloca i32, align 4
store i32 %0, i32* %2, align 4
%3 = load i32, i32* %2, align 4
%4 = icmp sle i32 %3, 1
br i1 %4, label %Label2, label %Label0
Label2:
ret i32 1
br label %Label0
Label0:
%6 = load i32, i32* %2, align 4
%7 = sub nsw i32 %6, 1
%8 = call noundef i32 @fac(i32 noundef %7)
%9 = load i32, i32* %2, align 4
%10 = mul nsw i32 %9, %8
ret i32 %10
}
define dso_local noundef i1 @isFacEven(i32 noundef %0) {
%2 = alloca i32, align 4
store i32 %0, i32* %2, align 4
%3 = load i32, i32* %2, align 4
%4 = call noundef i32 @fac(i32 noundef %3)
%5 = srem i32 %4, 2
%6 = icmp eq i32 %5, 0
br i1 %6, label %Label3, label %Label0
Label3:
ret i1 1
br label %Label1
Label0:
ret i1 0
br label %Label1
Label1:
ret i1 0
}
define dso_local noundef i32 @main() {
%1 = alloca i32, align 4
%2 = call noundef i32 @readInt()
store i32 %2, i32* %1, align 4
%3 = load i32, i32* %1, align 4
%4 = call noundef i1 @isFacEven(i32 noundef %3)
br i1 %4, label %Label3, label %Label0
Label3:
call void @printString(i8* noundef getelementptr ([17 x i8], [17 x i8]* @.str.1, i32 0, i32 0))
br label %Label1
Label0:
call void @printString(i8* noundef getelementptr ([16 x i8], [16 x i8]* @.str.2, i32 0, i32 0))
br label %Label1
Label1:
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
declare dso_local i8* @allocateInstance(i8* noundef %0, i32 noundef %1)
declare dso_local i8* @getField(i8* noundef %0, i32 noundef %1)
