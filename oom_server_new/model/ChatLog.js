import mongoose from 'mongoose';
const { Schema, model } = mongoose;

const chatLogsSchema = new Schema({
    chatSessionId: {
        type: Schema.Types.ObjectId,
        ref: 'ChatSessions',
    },
    // 이 항목을 참조시킬려면 코드가 복잡해지므로 채팅에 접속할 때 userId 를 통해 넘겨받는걸로
    sender: {
        type: String,
        required: true,
        maxlength: 3,
    },
    message: {
        type: String,
        required: true,
    },
    createdAt: {
        type: Date,
        default: () => Date.now(),
        immutable: true,
    },
});

const ChatLogs = model('ChatLogs', chatLogsSchema);
export default ChatLogs;