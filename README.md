# Real-time-Chroma-key
## A real-time Chroma Key project.
- Project này được tạo ra nhằm thực hiện việc chroma key real-time trong mọi trường hợp: camera đặt cố định (1) hoặc camera di chuyển (2).
- Hiện nay việc key trong trường hợp (1) đã hoàn thành, việc tiếp theo là làm với trường hợp (2).
## Ngôn ngữ và cấu hình
- Ngôn ngữ sử dụng là C++, dùng với thư viện OpenCV trên VS 2013.
- Project được config trên windows 7 64bit.
## Tiến độ công việc và hướng phát triển
Camera có 2 trường hợp khi di chuyển:
- Pan: di chuyển kiểu tịnh tiến, tức là cả bức ảnh sẽ + với 1 vector nào đó.
- Zoom: phóng to hay thu nhỏ nhưng tỉ lệ giữa các điểm quan trọng không thay đổi.
