import React, { useState, useEffect } from 'react';
import { useLocation } from 'react-router-dom';
import io from 'socket.io-client';
const socket = io('https://open-one-meal-server-e0778adebef6.herokuapp.com', {
        transports: ['websocket', 'polling'],
        withCredentials: true,
    });

const Chat = () => {
    const location = useLocation();
    const { userId } = location.state;

    const [messages, setMessages] = useState([]);
    const [input, setInput] = useState('');

    useEffect(() => {
        // 서버에 사용자 등록
        socket.emit('register', { userId });

        // 이전 메시지 불러오기
        socket.on('loadMessages', (chatLogs) => {
            console.log('메시지 로딩');
            setMessages(chatLogs);
        })

        // 상대로부터 메시지를 받음
        socket.on('receiveMessage', (message) => {
            console.log('메시지 수신', message);
            setMessages((prevMessages) => [...prevMessages, message]);
        });

        return () => {
            socket.emit('disconnect');
            socket.disconnect();
        };

    }, [userId]);

    const sendMessage = () => {
        console.log('메시지 전송', input);
        setMessages((prevMessages) => [...prevMessages, input]);
        socket.emit('sendMessage', input);
        setInput('');
    };

    return (
        <div>
          <div>
            {messages.map((msg, index) => (
              <div key={index}>{msg}</div>
            ))}
          </div>
          <input
            type="text"
            value={input}
            onChange={(e) => setInput(e.target.value)}
          />
          <button onClick={sendMessage}>Send</button>
        </div>
    );
};

export default Chat;