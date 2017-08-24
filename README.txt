XÂY DỰNG DỊCH VỤ MÃ HÓA VÀ GIẢI MÃ VĂN BẢN SỬ DỤNG THUẬT TOÁN HUFFMAN.
1.Yêu cầu môi trường:
	Ubuntu 64bit	
2.Cấu trúc của thư mục:
	+Thư mục lib chứa thư viện cho lập trình socket và thư viện phục vụ cho việc mã hóa giải mã.
	+Thư mục serverDB sẽ lưu filelog, bảng mã thu được khi mã hóa giải mã và các file tạm trong quá trình mã hóa giải mã( sau khi giải mã , mã hóa thành công thì chúng sẽ tự động xóa tránh làm tốn dung lượng ổ đĩa).
	+Thư mục clientDB dùng để lưu các file kết quả được trả về từ server.Nếu mã hóa thì file kết quả sẽ có đuôi là .zip , còn giải mã thì file kết quả sẽ có đuôi là .txt .Chú ý trong thư mục clientDB cũng đã có sẵn các file văn bản để test với độ lớn tăng dần như sau : hung < small < medium < big < super.
3.Hướng dẫn dịch :
	make client //nếu thành công sẽ cho ra 1 file 'client'
	make server //nếu thành công sẽ cho ra 1 file 'server'
4.Hướng dẫn chạy :
	+Phía SERVER: ở một terminal chạy lệnh : ./server [PORT]
		Nếu ta không truyền tham số PORT thì sẽ dùng port mặc định trong code là 1235.		
	+Phía CLIENT: ở terminal khác chạy lệnh : ./client IPSERVER [PORT]
		Nhập số 1 nếu muốn mã hóa, số 2 nếu muốn giải mã.
		Sau đó nhập tên file muốn mã hóa( hoặc giải mã) rồi enter và chờ kết quả.
		