# Real-time-Chroma-key
## A real-time Chroma Key project.
- Project này được tạo ra nhằm thực hiện việc chroma key real-time lên một màn hình TV (là màn hình xanh) tại một trường quay thật trong mọi trường hợp: camera đặt cố định (1) hoặc camera di chuyển (2).
- Hiện nay việc key trong trường hợp (1) đã hoàn thành, việc tiếp theo là làm với trường hợp (2).

## Ngôn ngữ và cấu hình
- Ngôn ngữ sử dụng là C++, dùng với thư viện OpenCV trên VS 2013.
- Project được config trên windows 7 64bit.

## Tiến độ công việc và hướng phát triển
Camera có 2 trường hợp khi di chuyển:
- Pan: di chuyển kiểu tịnh tiến, tức là cả bức ảnh sẽ + với 1 vector nào đó. Cần chú ý rằng ta chỉ quan tâm đến vị trí của màn hình xanh (TV ảo). Giải pháp: sử dụng 1 trong các hàm remap,  warpAffine hoặc getRotationMatrix2D.
- Zoom: phóng to hay thu nhỏ nhưng tỉ lệ khoảng cách giữa các điểm quan trọng (ở đây là 4 điểm góc so với biên chẳng hạn) không thay đổi. Trường hợp này coi như xong, OpenCV có hàm hỗ trợ việc zoom ảnh (pyrUp - zoom out; pyrDown - zoom in), cách làm là lấy tham số từ thiết bị, zoom in hay out bao nhiêu thì nhân ảnh chuẩn với hệ số tương ứng.

Nhìn chung cả 2 trường hợp đều cần làm trên thực nghiệm để dễ hình dung hơn.
