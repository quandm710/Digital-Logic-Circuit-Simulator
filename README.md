LOGIC SIMULATOR - CÔNG CỤ MÔ PHỎNG MẠCH LOGIC KỸ THUẬT SỐ
1. Tổng quan đề tài
- Dự án này tập trung xây dựng một ứng dụng tiện ích cho phép người dùng thiết kế và mô phỏng các mạch logic cơ bản trên nền tảng máy tính.
- Mục tiêu của nhóm là tạo ra một công cụ trực quan, giúp sinh viên ngành Kỹ thuật Điện - Điện tử và Công nghệ thông tin dễ dàng kiểm chứng các lý thuyết về đại số Boole và mạch số.
- Ngôn ngữ: C++ (chuẩn C++14 trở lên).
- Thư viện: Qt Framework (phiên bản 5.15+ hoặc 6.x).
- Hướng phát triển: Hướng 3 - Ứng dụng tiện ích / công cụ.
2. Tính năng nổi bật
- Giao diện đồ họa (GUI): Thiết kế trực quan, hỗ trợ kéo thả linh kiện và tương tác đa điểm trên scene.
- Mô phỏng động: Tín hiệu logic (0/1) được truyền dẫn và hiển thị theo thời gian thực khi có bất kỳ thay đổi nào từ đầu vào.
- Quản lý phiên làm việc: Hỗ trợ làm việc trên nhiều Tab cùng lúc.
- Hệ thống "Dirty Bit": Tự động theo dõi thay đổi và hiện dấu * trên tiêu đề khi mạch chưa được lưu.
- Hỏi ý kiến xác nhận (Lưu/Hủy) trước khi đóng Tab để tránh mất dữ liệu.
- Xử lý lưu trữ: Lưu và đọc dữ liệu mạch điện thông qua định dạng file .logic tùy chỉnh.
- Cơ chế ràng buộc: Chống xung đột dữ liệu bằng cách khóa tính năng chỉnh sửa thủ công trên các chân Input đã có dây nối.
3. Kỹ thuật lập trình đã áp dụng
- Để đáp ứng yêu cầu của môn học, nhóm đã áp dụng các kỹ thuật lập trình hướng đối tượng (OOP) trong C++ như sau:
  + Trừu tượng (Abstraction): Xây dựng lớp cổng logic cơ sở để định nghĩa các hành vi chung.
  + Kế thừa & Đa hình: Các lớp cổng AND, OR, NOT,... kế thừa từ lớp cơ sở và ghi đè (override) phương thức compute().
  + Quản lý bộ nhớ: Áp dụng nguyên tắc RAII và cơ chế deleteLater của Qt để quản lý vòng đời đối tượng an toàn.
  + Cấu trúc dữ liệu: Sử dụng STL Containers như QMap để quản lý ID linh kiện và QList để truy vết các kết nối dây nối.
  + Xử lý ngoại lệ: Kiểm soát các tình huống lỗi khi đọc file hoặc file bị sai định dạng dữ liệu.
4. Danh sách nhóm và Phân công công việc
- Nhóm thực hiện gồm 4 sinh viên.
- Thành viên || Nhiệm vụ chính
  + Quân     || Tạo GUI cơ bản, xử lý File I/O, tổng hợp và testing.
  + Quý      || Xây dựng logic các cổng, xây dựng dây nối và truyền tín hiệu giữa các cổng.
  + Thịnh    || Lên ý tưởng, tạo chức năng, tối ưu cửa sổ phần mềm, viết báo cáo.
  + Sang     || Tối ưu giao diện, cải tiến workspace, testing.
