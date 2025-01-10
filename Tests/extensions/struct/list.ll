%class.list = type {i32,%class.list*}
define dso_local noundef i32 @main() {
%1 = call noundef %class.list* @fromTo(i32 noundef 1,i32 noundef 50)
%2 = call noundef i32 @length(%class.list* noundef %1)
call void @printInt(i32 noundef %2)
%3 = call noundef %class.list* @fromTo(i32 noundef 1,i32 noundef 100)
%4 = call noundef i32 @length2(%class.list* noundef %3)
call void @printInt(i32 noundef %4)
ret i32 0
}
define dso_local noundef i32 @head(%class.list* noundef %0) {
%2 = alloca %class.list*, align 8
store %class.list* %0, %class.list** %2, align 8
%3 = load %class.list*, %class.list** %2, align 8
%4 = getelementptr inbounds %class.list, %class.list* %3, i32 0, i32 0
%5 = load i32, i32* %4, align 4
ret i32 %5
}
define dso_local noundef %class.list* @cons(i32 noundef %0, %class.list* noundef %1) {
%3 = alloca i32, align 4
%4 = alloca %class.list*, align 8
%5 = alloca %class.list*, align 8
store i32 %0, i32* %3, align 4
store %class.list* %1, %class.list** %4, align 8
store %class.list* null, %class.list** %5, align 8
%6 = call i8* @allocateInstance(i32 noundef 12)
%7 = bitcast i8* %6 to %class.list*
store %class.list* %7, %class.list** %5, align 8
%8 = load %class.list*, %class.list** %5, align 8
%9 = getelementptr inbounds %class.list, %class.list* %8, i32 0, i32 0
%10 = load i32, i32* %3, align 4
store i32 %10, i32* %9, align 4
%11 = load %class.list*, %class.list** %5, align 8
%12 = getelementptr inbounds %class.list, %class.list* %11, i32 0, i32 1
%13 = load %class.list*, %class.list** %4, align 8
store %class.list* %13, %class.list** %12, align 8
%14 = load %class.list*, %class.list** %5, align 8
ret %class.list* %14
}
define dso_local noundef i32 @length(%class.list* noundef %0) {
%2 = alloca %class.list*, align 8
store %class.list* %0, %class.list** %2, align 8
%3 = load %class.list*, %class.list** %2, align 8
%4 = icmp eq %class.list* %3, null
br i1 %4, label %Label3, label %Label0
Label3:
ret i32 0
br label %Label1
Label0:
%6 = load %class.list*, %class.list** %2, align 8
%7 = getelementptr inbounds %class.list, %class.list* %6, i32 0, i32 1
%8 = load %class.list*, %class.list** %7, align 8
%9 = call noundef i32 @length(%class.list* noundef %8)
%10 = add nsw i32 1, %9
ret i32 %10
br label %Label1
Label1:
ret i32 0
}
define dso_local noundef %class.list* @fromTo(i32 noundef %0, i32 noundef %1) {
%3 = alloca i32, align 4
%4 = alloca i32, align 4
store i32 %0, i32* %3, align 4
store i32 %1, i32* %4, align 4
%5 = load i32, i32* %3, align 4
%6 = load i32, i32* %4, align 4
%7 = icmp sgt i32 %5, %6
br i1 %7, label %Label3, label %Label0
Label3:
ret %class.list* null
br label %Label1
Label0:
%9 = load i32, i32* %3, align 4
%10 = add nsw i32 %9, 1
%11 = load i32, i32* %4, align 4
%12 = call noundef %class.list* @fromTo(i32 noundef %10,i32 noundef %11)
%13 = load i32, i32* %3, align 4
%14 = call noundef %class.list* @cons(i32 noundef %13,%class.list* noundef %12)
ret %class.list* %14
br label %Label1
Label1:
ret %class.list* null
}
define dso_local noundef i32 @length2(%class.list* noundef %0) {
%2 = alloca %class.list*, align 8
%3 = alloca i32, align 4
store %class.list* %0, %class.list** %2, align 8
store i32 0, i32* %3, align 4
br label %Label0
Label0:
%4 = load %class.list*, %class.list** %2, align 8
%5 = icmp ne %class.list* %4, null
br i1 %5, label %Label3, label %Label1
Label3:
%6 = load i32, i32* %3, align 4
%7 = add nsw i32 %6, 1
store i32 %7, i32* %3, align 4
%8 = load %class.list*, %class.list** %2, align 8
%9 = getelementptr inbounds %class.list, %class.list* %8, i32 0, i32 1
%10 = load %class.list*, %class.list** %9, align 8
store %class.list* %10, %class.list** %2, align 8
br label %Label0
Label1:
%11 = load i32, i32* %3, align 4
ret i32 %11
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
declare dso_local i8* @allocateInstance(i32 noundef %0)
declare dso_local i8* @getField(i8* noundef %0, i32 noundef %1)
