// mongoose 의 기능 중 하나인 스키마 모델 정의 예시 (User)
import mongoose from 'mongoose';
const { Schema, model } = mongoose;

// Validators only run on the create or save methods
// Create 또는 Save 메서드로만 유효성 검사기(type, required,.. )가 실행된다
const usersSchema = new Schema({
    name: {
        type: String,
        required: true,
        maxlength: 3,
    },
    gender: {
        type: String,
        required: true,
        maxlength: 2,
    },
    email: {
        type: String,
        // required: 누락 하면 안되는 필드
        required: true,
        maxlength: 30,
    },
    password: {
        type: String,
        required: true,
        maxlength: 60,
    },
    createdAt: {
        type: Date,
        default: () => Date.now(),
        // immutable: 첫 할당 이후 수정 불가능한 필드
        immutable: true,
    },
    updatedAt: Date,
    chatSessionId: {
        type: Schema.Types.ObjectId,
        ref: 'ChatSessions',
    },
});

// Middleware
// 미들웨어는 실행 전 또는 실행 중에 비동기적으로 처리되는 함수
// 이 미들웨어는 save 메서드를 이용했을 때, 자동으로 updatedAt 필드를 현재 날짜로 갱신함
usersSchema.pre('save', function(next) {
    this.updatedAt = Date.now();
    next();
});

// 외부에서 import 했을 때 사용할 수 있는 모델이며,
// User 로 접근이 이 스키마에 접근이 가능함.
const Users = model('Users', usersSchema);
export default Users;