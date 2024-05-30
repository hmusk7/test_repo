# 오픈한끼 프로젝트 Server Repository

> 📅 Update Date: 2024-05-27
> 

## 1. Overview

이 리포지토리는 오픈한끼 프로젝트 서버 구현을 위해 노력한 과정과 코드를 제공합니다.

### 1.1. 서버 구현 준비

- Node.js 설치 후 npm 환경 변수 등록
- MongoDB Atlas 에 Organization, Project, Cluster 생성
- MongoDB Compass 및 mongosh 설치
- 서버 코드를 저장할 폴더를 생성 후 VS Code 터미널 실행
- npm 모듈 설치: express, mongoose, nodemon, axios
    1. `npm init -y`
    2. `npm i express mongoose axios`
    3. `npm i -D nodemon`

## 2. MongoDB

### 2.1. MongoDB 제품 및 라이브러리 설명

- MongoDB Atlas: MongoDB 에서 제공하는 클라우드 데이터베이스 서비스
- MongoDB Compass: 데이터베이스 관리에 GUI 환경을 제공하는 프로그램
- MongoDB Shell (mongosh): 터미널에서 데이터베이스 관리 기능을 제공하는 프로그램
- Mongoose: Node.js 환경에서 아주 쉽게 MongoDB 객체 모델링을 지원하는 라이브러리. 복잡한 데이터 모델이나 관계형 모델을 생각할 필요 없이 JavaScript 파일에 스키마 모델을 추가하고, 서버 코드에서 import 하여 스키마에 따라 Document 를 읽거나 쓸 수 있습니다.

### 2.2. 클러스터명

- Production 환경을 위한 클러스터: ProductionCluster
    
    실제 운영중인 서버에는 ProductionCluster 를 사용합니다.
    
- Development 환경을 위한 클러스터: DevelopmentCluster
    
    개발 중인 서버에는 DevelopmentCluster 를 사용합니다.
    

### 2.3. mongoose 코드

- mongoose 임포트 및 DB 연결
    
    ```jsx
    // CommonJS 방식
    const mongoose = require('mongoose'); // (1)
    // ES Module 방식
    // package.json 에 "type": "module" 명시
    import mongoose from 'mongoose'       // (2)
    
    // mongoose 를 이용해 DB 에 연결
    mongoose.connect("MongoDB Atlas의 DB 주소");
    ```
    
- 컬렉션의 스키마 설정
    
    ```jsx
    // Users.js
    import mongoose from 'mongoose';
    const { Schema, model } = mongoose;
    
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
            maxlength: 50
        },
        password: {
            type: String,
            required: true,
        },
        createdAt: {
            type: Date,
            default: () => Date.now(),
            // immutable: 첫 할당 이후 수정 불가능한 필드
            immutable: true,
        },
        updatedAt: Date,
    });
    
    // save 메서드를 이용했을 때 자동으로 updatedAt 필드를 현재 날짜로 갱신하는 미들웨어
    usersSchema.pre('save', function(next) {
        this.updatedAt = Date.now();
        next();
    });
    
    // 외부에서 import 했을 때 사용할 수 있는 모델이며,
    // User 로 접근이 이 스키마에 접근이 가능함.
    const Users = model('Users', usersSchema);
    export default Users;
    ```
    
- DB 와 통신
    
    ```jsx
    // Document 생성 및 저장
    // 1번 방법: 인스턴스 생성 후 save()
    const user = new User({ email: 'test@test.com' });
    await user.save();
    // 2번 방법: 인스턴스 생성 및 저장을 한꺼번에 수행해주는 create()
    const user = await User.create({ email: 'test@test.com' }) //
    
    // Update (갱신)
    // 방법: . 으로 필드를 직접 참조해서 값 수정 후 save()
    user.email = "test@test.com"
    await user.save();
    
    // Find (조회)
    // 1번 방법: findOne(), findMany()
    const user = await User.findOne({ name: "비르츠" })
    const user = await User.findMany({ name: ["그리말도", "시크"] })
    // 2번 방법: findById()
    const user = await User.findById("ID 번호").exec();
    // 3번 방법: .where().equals().select()
    // where 은 필드를 필터링, select 는 보여줄 필드(projection)를 결정
    const user = await User.where("email").eqauls("test@test.com").select("name");
    
    // Delete (삭제)
    // 방법: deleteOne(), deleteMany()
    const user = await User.deleteMany({ email: ["test@test.com", "test2@test.com"] });
    ```
    

## 3. 서버 구현

### 3.1. 미들웨어 설정

- JSON 객체 처리, CORS 사전 요청 처리를 위한 미들웨어 설정
    - JSON 객체 처리: 자바스크립트는 JSON 객체로 클라이언트와 서버가 통신. 예를 들어, `fetch` 또는 `axios` 를 이용한 요청은 전부 JSON 객체이며, 이를 파싱하여 자바스크립트 객체로 변환해줄 미들웨어를 설정
    - CORS 사전 요청 처리: 클라이언트는 서버로 실제 요청을 보내기 이전 CORS 사전 요청을 보냄. 서버가 이 요청을 허용해야 클라이언트가 실제 요청을 보낼 수 있으므로, CORS 사전 요청을 허용하도록 미들웨어를 설정
    
    ```jsx
    // express.json() 미들웨어를 통해 JSON 형식의 요청 본문이 알아서 파싱됨
    app.use(express.json())
    
    import cors from 'cors';
    // 허용할 CORS 의 헤더, 메서드, origin 까지 직접 명시할 수도 있으나,
    // 이렇게 설정하면 모든 CORS 사전 요청을 허용함.
    app.use(cors())
    // 클라이언트가 보내는 사전 요청은 OPTION 메서드이며,
    // 이렇게 하면 서버에 존재하는 모든 경로에 대하여 CORS 가 허용됨
    app.options('*', cors());
    ```
    

### 3.2. 로그인 기능

- 로그인 기능은 클라이언트로부터 이메일과 패스워드를 JSON 객체로 받아서, 미들웨어를 통해 파싱하고, 객체의 이메일 필드의 값을 DB 에서 조회하여 일치하는 이메일이 있는지 확인하고, 패스워드도 일치하는지 확인함. 패스워드는 `bcrypt.compare()` 함수를 이용해 입력 받은 평문 패스워드를 해시하여 DB 에 저장된 해시된 패스워드와 비교를 수행함.
- 클라이언트는 axios 의 POST 요청
    
    ```jsx
    const response = await axios.post(
    	"URL/api/signin",
    	sendData
    );
    ```
    
- 서버는 express 의 POST 응답
    
    ```jsx
    app.post('/api/signin', async (req, res) => {
        const { email, password } = req.body;
    
        try {
            const user = await User.findOne({ email });
    
            if (!user) {
                return res.status(401).json({status: 401, message: '이메일 혹은 패스워드가 일치하지 않습니다.'});
            }
            
            const isMatch = await bcrypt.compare(password, user.password);
    
            if (!isMatch) {
                return res.status(401).json({status: 401, message: '이메일 혹은 패스워드가 일치하지 않습니다.'});
            }
    
            return res.status(200).json({ status: 200, message: '로그인 성공' });
        } catch (err) {
            return res.status(500).json({ status: 500, message: 'Server error'});
        }
    });
    ```
    

### 3.3. 회원가입 기능

- 클라이언트로부터 회원가입 폼에 입력된 값들이 저장된 JSON 객체를 넘겨받아, 이메일이 중복됐는지 검사하고, 중복되지 않았으면 패스워드를 해시화한 뒤 DB 에 계정을 생성함
- 클라이언트는 axios 의 POST 요청
    
    ```jsx
    const response = await axios.post(
    	"URL/api/signup",
    	sendData
    );
    ```
    
- 서버는 express 의 POST 응답
    
    ```jsx
    app.post('/api/signup', async (req, res) => {
        const { name, gender, email, password } = req.body;
    
        try {
            
            const user = await Users.findOne({ email });
            if (user) {
                return res.status(409).json({status:409, message: '중복된 이메일'});
            }
    
            // bcrypt.hash 를 통해 전달받은 패스워드를 해시화함
            // 두번째 매개인자인 숫자는 해시 레벨이며,
            // 해시 레벨을 높일 수록 처리 속도는 느려지지만 그만큼 복잡한 해시화
            const hashedPassword = await bcrypt.hash(password, 10);
    
            await Users.create({
                name: name,
                gender: gender,
                email: email,
                password: hashedPassword,
            })
            
            res.status(201).send('계정 생성 성공');
        } catch (error) {
            res.status(500).send('계정 생성 중 오류 발생');
        }
    });
    ```
    

## 4. Heroku 서버 호스팅

### 4.1. 서버 호스팅 준비

- Heroku 계정 생성
- Heroku CLI 설치
- Heroku 애플리케이션 생성
- 예방적 차원으로 package.json 의 dependencies 를 `npm install` 로 전부 설치

### 4.2. 환경 변수 설정

- Heroku 환경 변수에 MongoDB Atlas 의 Connect 주소를 등록하고,
    
    `mongoose.connect(process.env.MONGODB_URI);` 와 같이 환경 변수를 사용하여 DB에 연결
    
- 서버를 열 때는 Heroku 의 환경 변수에 등록된 포트 번호를 이용
    
    `app.listen(process.env.PORT, () ⇒ {});`
    

### 4.3. 엔트리 포인트 명시

- package.json 의 `“scripts”` 섹션에 `“start”: “node index.js”` 추가
- Procfile 파일 생성
    - Heroku 는 Procfile 을 사용하여 애플리케이션을 시작함
    - 전체 프로젝트의 최상위 디렉터리에 Procfile 을 확장자 없이 생성하고, 실행할 명령을 추가
    
    ```jsx
    // Procfile
    web: node index.js
    ```
    

### 4.4. MongoDB Atlas 의 Network Access IP 설정

- Heroku 호스팅 서비스의 IP 의 접근을 허용해야 하는데, Heroku 는 유료 버전을 사용하지 않으면 동적인 IP 주소를 할당하기 때문에 전체 접근을 허용하도록 설정

### 4.5. Github 에 Push 후 Heroku 에 Deploy

- Heroku 에 Deploy 를 간소화하기 위해, Github 리포에 서버 코드를 전부 업로드하고 Heroku 에서 Github 의 리포를 이용해 서버를 호스팅하도록 설정

### 4.6. 서버 실행

- `heroku ps:scale web=1` 설정하여 서버 실행
- `heroku ps:scale web=0` 서버 종료
- `heroku ps:restart` 서버 재시작
- `heroku logs --tail` 서버 로그 확인