R #(.R(4k), .Temp(26.85), .Tc1(0.0), .Tc2(0.0), .Tnom(26.85), .Symbol(european)) R1(net_380_160, net_440_160);
C #(.C(4p), .V(), .Symbol(neutral)) C1(net_440_160, net_500_160);
Port #(.Num(3), .Type()) P3(net_50_60);
Port #(.Num(4), .Type()) P4(net_50_400);
Port #(.Num(5), .Type()) P6(net_540_230);
Port #(.Num(1), .Type()) P1(net_50_230);
Port #(.Num(2), .Type()) P2(net_50_170);
Idc #(.I(5 uA)) I1(net_90_300, net_90_240);
wire #() noname(net_200_100, net_210_100);
wire #() noname(net_170_130, net_170_140);
wire #() noname(net_310_130, net_310_160);
wire #() noname(net_190_230, net_270_230);
wire #() noname(net_170_260, net_170_280);
wire #() noname(net_310_260, net_310_280);
wire #() noname(net_170_280, net_240_280);
wire #() noname(net_160_350, net_180_350);
wire #() noname(net_240_280, net_310_280);
wire #() noname(net_240_280, net_240_320);
wire #() noname(net_130_380, net_130_400);
wire #() noname(net_240_380, net_240_400);
wire #() noname(net_130_400, net_240_400);
wire #() noname(net_170_60, net_170_70);
wire #() noname(net_170_60, net_310_60);
wire #() noname(net_310_60, net_310_70);
wire #() noname(net_130_300, net_130_320);
wire #() noname(net_180_350, net_210_350);
wire #() noname(net_180_300, net_180_350);
wire #() noname(net_130_300, net_180_300);
wire #() noname(net_440_300, net_440_350);
wire #() noname(net_180_300, net_440_300);
wire #() noname(net_440_350, net_470_350);
wire #() noname(net_500_380, net_500_400);
wire #() noname(net_240_400, net_270_400);
wire #() noname(net_340_230, net_360_230);
wire #() noname(net_310_160, net_310_200);
wire #() noname(net_310_160, net_380_160);
wire #() noname(net_500_230, net_500_320);
wire #() noname(net_500_230, net_540_230);
wire #() noname(net_270_230, net_290_230);
wire #() noname(net_270_400, net_500_400);
wire #() noname(net_270_230, net_270_400);
wire #() noname(net_170_140, net_170_200);
wire #() noname(net_170_140, net_210_140);
wire #() noname(net_210_100, net_280_100);
wire #() noname(net_210_100, net_210_140);
wire #() noname(net_500_160, net_500_230);
wire #() noname(net_380_100, net_470_100);
wire #() noname(net_380_100, net_380_160);
wire #() noname(net_500_130, net_500_160);
wire #() noname(net_500_60, net_500_70);
wire #() noname(net_310_60, net_500_60);
wire #() noname(net_90_300, net_130_300);
wire #() noname(net_90_60, net_170_60);
wire #() noname(net_90_60, net_90_240);
wire #() noname(net_50_60, net_90_60);
wire #() noname(net_50_400, net_130_400);
wire #() noname(net_50_230, net_140_230);
wire #() noname(net_50_170, net_360_170);
wire #() noname(net_360_170, net_360_230);
place #(.$xposition(50),.$yposition(60)) place_50_60(net_50_60);
place #(.$xposition(50),.$yposition(170)) place_50_170(net_50_170);
place #(.$xposition(50),.$yposition(230)) place_50_230(net_50_230);
place #(.$xposition(50),.$yposition(400)) place_50_400(net_50_400);
place #(.$xposition(90),.$yposition(60)) place_90_60(net_90_60);
place #(.$xposition(90),.$yposition(240)) place_90_240(net_90_240);
place #(.$xposition(90),.$yposition(300)) place_90_300(net_90_300);
place #(.$xposition(130),.$yposition(300)) place_130_300(net_130_300);
place #(.$xposition(130),.$yposition(320)) place_130_320(net_130_320);
place #(.$xposition(130),.$yposition(380)) place_130_380(net_130_380);
place #(.$xposition(130),.$yposition(400)) place_130_400(net_130_400);
place #(.$xposition(140),.$yposition(230)) place_140_230(net_140_230);
place #(.$xposition(160),.$yposition(350)) place_160_350(net_160_350);
place #(.$xposition(170),.$yposition(60)) place_170_60(net_170_60);
place #(.$xposition(170),.$yposition(70)) place_170_70(net_170_70);
place #(.$xposition(170),.$yposition(130)) place_170_130(net_170_130);
place #(.$xposition(170),.$yposition(140)) place_170_140(net_170_140);
place #(.$xposition(170),.$yposition(200)) place_170_200(net_170_200);
place #(.$xposition(170),.$yposition(260)) place_170_260(net_170_260);
place #(.$xposition(170),.$yposition(280)) place_170_280(net_170_280);
place #(.$xposition(180),.$yposition(300)) place_180_300(net_180_300);
place #(.$xposition(180),.$yposition(350)) place_180_350(net_180_350);
place #(.$xposition(190),.$yposition(230)) place_190_230(net_190_230);
place #(.$xposition(200),.$yposition(100)) place_200_100(net_200_100);
place #(.$xposition(210),.$yposition(100)) place_210_100(net_210_100);
place #(.$xposition(210),.$yposition(140)) place_210_140(net_210_140);
place #(.$xposition(210),.$yposition(350)) place_210_350(net_210_350);
place #(.$xposition(240),.$yposition(280)) place_240_280(net_240_280);
place #(.$xposition(240),.$yposition(320)) place_240_320(net_240_320);
place #(.$xposition(240),.$yposition(380)) place_240_380(net_240_380);
place #(.$xposition(240),.$yposition(400)) place_240_400(net_240_400);
place #(.$xposition(270),.$yposition(230)) place_270_230(net_270_230);
place #(.$xposition(270),.$yposition(400)) place_270_400(net_270_400);
place #(.$xposition(280),.$yposition(100)) place_280_100(net_280_100);
place #(.$xposition(290),.$yposition(230)) place_290_230(net_290_230);
place #(.$xposition(310),.$yposition(60)) place_310_60(net_310_60);
place #(.$xposition(310),.$yposition(70)) place_310_70(net_310_70);
place #(.$xposition(310),.$yposition(130)) place_310_130(net_310_130);
place #(.$xposition(310),.$yposition(160)) place_310_160(net_310_160);
place #(.$xposition(310),.$yposition(200)) place_310_200(net_310_200);
place #(.$xposition(310),.$yposition(260)) place_310_260(net_310_260);
place #(.$xposition(310),.$yposition(280)) place_310_280(net_310_280);
place #(.$xposition(340),.$yposition(230)) place_340_230(net_340_230);
place #(.$xposition(360),.$yposition(170)) place_360_170(net_360_170);
place #(.$xposition(360),.$yposition(230)) place_360_230(net_360_230);
place #(.$xposition(380),.$yposition(100)) place_380_100(net_380_100);
place #(.$xposition(380),.$yposition(160)) place_380_160(net_380_160);
place #(.$xposition(440),.$yposition(160)) place_440_160(net_440_160);
place #(.$xposition(440),.$yposition(300)) place_440_300(net_440_300);
place #(.$xposition(440),.$yposition(350)) place_440_350(net_440_350);
place #(.$xposition(470),.$yposition(100)) place_470_100(net_470_100);
place #(.$xposition(470),.$yposition(350)) place_470_350(net_470_350);
place #(.$xposition(500),.$yposition(60)) place_500_60(net_500_60);
place #(.$xposition(500),.$yposition(70)) place_500_70(net_500_70);
place #(.$xposition(500),.$yposition(130)) place_500_130(net_500_130);
place #(.$xposition(500),.$yposition(160)) place_500_160(net_500_160);
place #(.$xposition(500),.$yposition(230)) place_500_230(net_500_230);
place #(.$xposition(500),.$yposition(320)) place_500_320(net_500_320);
place #(.$xposition(500),.$yposition(380)) place_500_380(net_500_380);
place #(.$xposition(500),.$yposition(400)) place_500_400(net_500_400);
place #(.$xposition(540),.$yposition(230)) place_540_230(net_540_230);
