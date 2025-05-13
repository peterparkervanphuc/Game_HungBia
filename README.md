# 🎮 Game Hứng Bia

## 📝 Thông tin cá nhân
- **Họ và tên**: Đào Ngọc Duy  
- **Mã số sinh viên**: 24022637  
- **Lớp**: K69I-IS4  
- **Chủ đề**: Bài tập lớn (làm cá nhân)  

## 📝 Mô tả
Game Hứng Bia là một trò chơi giải trí đơn giản, nơi người chơi điều khiển một thùng bia để hứng những chai bia đang rơi xuống. 

## 🎯 Cách chơi
- Sử dụng phím mũi tên **← →** để di chuyển thùng bia
- Hứng được chai bia +100 điểm
- Bỏ lỡ một chai bia sẽ thua trò chơi (trừ khi có shield)
- Ấn **Space** để kích hoạt tăng tốc (sau 60 điểm)
- Đạt 110 điểm để hoàn thành game

## 🎨 Tính năng
- 3 nhân vật để lựa chọn
- Hệ thống tăng tốc (Speed Boost)
- Hệ thống nhân đôi điểm (x2 Score)
- Hệ thống khiên bảo vệ (Shield)
- Hệ thống làm chậm (Slow Effect)
- Âm thanh sinh động
- Độ khó tăng dần theo điểm số
- Hiệu ứng đặc biệt khi chiến thắng

## 🛡️ Các mốc và item đặc biệt
- 37 điểm: Icon x2 xuất hiện (kéo dài 8 giây, spawn lại sau 25 giây)
- 60 điểm: Mở khóa Speed Boost (ấn Space để kích hoạt)
- 76 điểm: Icon slow xuất hiện (kéo dài 8 giây, spawn lại sau 35 giây, giảm 40% tốc độ bia)
- 100 điểm: Shield xuất hiện (bảo vệ 2 lần bỏ lỡ, kéo dài 12 giây, spawn lại sau 35 giây)
- 110 điểm: Hoàn thành game

## 🔧 Công nghệ sử dụng
- SDL2
- SDL2_image 
- SDL2_ttf
- SDL2_mixer
- C++

## 🎵 Âm thanh
- Nhạc nền khi ở màn hình chính
- Hiệu ứng âm thanh khi hứng được bia
- Hiệu ứng âm thanh khi thua

## 🎮 Điều khiển
- **←**: Di chuyển sang trái
- **→**: Di chuyển sang phải 
- **Space**: Kích hoạt tăng tốc (sau khi đạt 60 điểm)

## 🏆 Chiến thuật
- Sử dụng Speed Boost để di chuyển nhanh hơn trong tình huống nguy cấp
- Tận dụng x2 Score để tăng điểm nhanh
- Shield giúp bạn an toàn khi sắp thua
- Slow Effect giúp dễ dàng hứng chai bia hơn khi có nhiều chai cùng lúc
