# Hệ thống IoT đếm người ứng dụng cảm biến PIR và mạng Zigbee 3.0

# Giới thiệu dự án
Chào mừng bạn đến với dự án Hệ thống IoT đếm người sử dụng cảm biến PIR và mạng Zigbee 3.0! Dự án này được tạo ra nhằm mục đích tận dụng công nghệ truyền thông không dây Zigbee và các vi điều khiển hiện đại để xây dựng một hệ thống đếm người chính xác, tiết kiệm năng lượng và có khả năng tích hợp sâu vào hệ sinh thái nhà thông minh.

# Tổng quan dự án
Dự án tập trung vào việc phát triển một hệ thống đếm người tự động sử dụng hai cảm biến PIR đặt tại lối ra vào, kết hợp với một bộ xử lý trung tâm và một bo hiển thị. Hệ thống có khả năng phân biệt người đi vào và đi ra dựa trên thứ tự kích hoạt của hai cảm biến, đồng thời báo cáo trạng thái phòng có người hay trống lên ứng dụng Lumi Life thông qua mạng Zigbee 3.0. Mục tiêu là tạo ra một giải pháp đơn giản, chi phí thấp nhưng vẫn đáp ứng được các yêu cầu thực tế trong quản lý không gian, tự động hóa thiết bị điện và nâng cao trải nghiệm người dùng cuối.

# Chức năng chính
1. Đếm người tự động: Hệ thống đếm chính xác số người trong phòng theo thời gian thực dựa trên việc phân tích cặp tín hiệu từ hai cảm biến PIR (1→2 cho người vào, 2→1 cho người ra).
2. Hiển thị trực quan: Số người và trạng thái phòng được hiển thị tức thì trên màn hình LCD ST7735, kèm theo đèn LED RGB báo trạng thái và buzzer cảnh báo khi có sự kiện.
3. Giám sát từ xa qua Lumi Life: Mọi trạng thái của hệ thống đều được đồng bộ với ứng dụng Lumi Life trên điện thoại, cho phép người dùng theo dõi và điều khiển từ bất cứ đâu có kết nối Internet.
4. Điều khiển ngược: Người dùng có thể gửi lệnh OFF từ ứng dụng để reset số người về 0, tắt LED, hoặc bật/tắt LED của các thiết bị cảm biến trực tiếp từ giao diện app.

# Video demo
https://drive.google.com/file/d/1hje_whT2oYBGZ6rEDbtNxD6O5sJagGN-/view?usp=sharing

# Hướng dẫn cài đặt
Bạn có thể tìm thấy hướng dẫn chi tiết về cách cài đặt và triển khai dự án trong thư mục Instructions, bao gồm các bước flash firmware cho EFR32 qua Simplicity Studio v4 và STM32 qua STM32CubeIDE, cùng với quy trình thiết lập binding giữa các thiết bị Zigbee. Xin vui lòng tham khảo tài liệu này để bắt đầu.

# Các tính năng sẽ phát triển
Dự án này sử dụng cơ chế binding của Zigbee và hai cảm biến PIR để xác định hướng di chuyển. Do hạn chế của phương pháp này khi nhiều người đi qua cùng lúc hoặc đi quá nhanh (cách nhau dưới 1 giây), dự án còn một số tính năng chưa được phát triển như đếm chính xác đoàn người, phân biệt người lớn với trẻ em, theo dõi quỹ đạo di chuyển trong phòng, hay tự động bật/tắt thiết bị điện theo số người. Nếu có thể nâng cấp lên các phương án cảm biến tiên tiến hơn (camera AI với model YOLO, cảm biến Time-of-Flight, cảm biến cắt tia hồng ngoại đối xứng), các tính năng kể trên sẽ được phát triển để tạo nên một hệ thống đếm người hoàn thiện và chính xác hơn trong mọi điều kiện sử dụng.
