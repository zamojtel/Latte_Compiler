%class.C = type {i8*}
@.str.1 = private unnamed_addr constant [6 x i8] c"\00\00\00\00\00\00", align 1
@.str.3 = private unnamed_addr constant [16 x i8] c"\00\00\00\00\00Jest empty\00", align 1
@.str.2 = private unnamed_addr constant [11 x i8] c"\00\00\00\00\00Po .s\00", align 1
@.str.5 = private unnamed_addr constant [20 x i8] c"\00\00\00\00\00c.s jest empty\00", align 1
@.str.7 = private unnamed_addr constant [18 x i8] c"\00\00\00\00\00s jest empty\00", align 1
@.vtable.C = dso_local global [1 x i8*] [i8* bitcast (i1 (%class.C*)* @C.empty to i8*)], align 16
define dso_local noundef i32 @main() {
%1 = alloca %class.C*, align 8
%2 = alloca i8*, align 8
%3 = call i8* @allocateInstance(i8* noundef bitcast ([1 x i8*]* @.vtable.C to i8*),i32 noundef 8)
%4 = bitcast i8* %3 to %class.C*
store %class.C* %4, %class.C** %1, align 8
%5 = getelementptr inbounds [6 x i8], [6 x i8]* @.str.1, i32 0, i32 0
store i8* %5, i8** %2, align 8
%6 = load %class.C*, %class.C** %1, align 8
%7 = getelementptr inbounds %class.C, %class.C* %6, i32 0, i32 0
%8 = load i8*, i8** %7, align 8
call void @printString(i8* noundef %8)
call void @printString(i8* noundef getelementptr ([11 x i8], [11 x i8]* @.str.2, i32 0, i32 0))
%9 = load %class.C*, %class.C** %1, align 8
%10 = call noundef i1 @C.empty(%class.C* noundef %9)
br i1 %10, label %Label4, label %Label0
Label4:
call void @printString(i8* noundef getelementptr ([16 x i8], [16 x i8]* @.str.3, i32 0, i32 0))
br label %Label0
Label0:
%11 = load %class.C*, %class.C** %1, align 8
%12 = getelementptr inbounds %class.C, %class.C* %11, i32 0, i32 0
%13 = load i8*, i8** %12, align 8
%14 = getelementptr inbounds [6 x i8], [6 x i8]* @.str.1, i32 0, i32 0
%15 = call zeroext i1 @compareStrings(i8* noundef %13, i8* noundef %14)
br i1 %15, label %Label5, label %Label1
Label5:
call void @printString(i8* noundef getelementptr ([20 x i8], [20 x i8]* @.str.5, i32 0, i32 0))
br label %Label1
Label1:
%16 = load i8*, i8** %2, align 8
%17 = getelementptr inbounds [6 x i8], [6 x i8]* @.str.1, i32 0, i32 0
%18 = call zeroext i1 @compareStrings(i8* noundef %16, i8* noundef %17)
br i1 %18, label %Label6, label %Label2
Label6:
call void @printString(i8* noundef getelementptr ([18 x i8], [18 x i8]* @.str.7, i32 0, i32 0))
br label %Label2
Label2:
ret i32 0
}
define dso_local noundef i1 @C.empty(%class.C* noundef %0) {
%2 = alloca %class.C*, align 8
store %class.C* %0, %class.C** %2, align 8
%3 = load %class.C*, %class.C** %2, align 8
%4 = getelementptr inbounds %class.C, %class.C* %3, i32 0, i32 0
%5 = load i8*, i8** %4, align 8
%6 = getelementptr inbounds [6 x i8], [6 x i8]* @.str.1, i32 0, i32 0
%7 = call zeroext i1 @compareStrings(i8* noundef %5, i8* noundef %6)
ret i1 %7
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
