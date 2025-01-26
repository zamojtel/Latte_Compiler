@.str.1 = private unnamed_addr constant [6 x i8] c"\00\00\00\00\00\00", align 1
@.str.2 = private unnamed_addr constant [8 x i8] c"\00\00\00\00\00pa\00", align 1
define dso_local noundef i32 @main() {
%1 = alloca i8*, align 8
%2 = getelementptr inbounds [6 x i8], [6 x i8]* @.str.1, i32 0, i32 0
store i8* %2, i8** %1, align 8
%3 = getelementptr inbounds [8 x i8], [8 x i8]* @.str.2, i32 0, i32 0
%4 = add nsw i8* %3, 3
store i8* %4, i8** %1, align 8
%5 = load i8*, i8** %1, align 8
call void @printString(i8* noundef %5)
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
