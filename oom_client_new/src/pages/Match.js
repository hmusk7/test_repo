import React from 'react';
import axios from 'axios';
import { useNavigate, useLocation } from 'react-router-dom';

const Match = () => {
  const nav = useNavigate();
  const location = useLocation();
  const { userId } = location.state;

  const handleMatch = async () => {
    try {
      const response = await axios.put(
        "https://open-one-meal-server-e0778adebef6.herokuapp.com/api/match",
        { userId : userId }
      );

      if (response.status === 200) {
        alert('매칭 완료');
        nav('/chat', {
          state: { userId: userId },
      });}
      
      else if (response.status === 204) {
        alert('이미 매칭됨');
        nav('/chat', {
          state: { userId: userId },
      });}
      
    } catch (error) {
      alert('매칭 오류 발생, 매칭 실패');
    }
  };

  return (
    <button onClick={handleMatch}>매칭 요청</button>
  );
};

export default Match;