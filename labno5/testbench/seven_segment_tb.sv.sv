

module seven_segment_tb;
    logic [3:0] num;
    logic C1; 
    logic C2; 
    logic C3; 
    logic C4; 
    logic C5; 
    logic C6; 
    logic C7;
    logic [7:0] an;

    seven_segment uut (
        .num(num),
        .sel(3'b000),
        .C1(C1), 
        .C2(C2), 
        .C3(C3), 
        .C4(C4),
        .C5(C5), 
        .C6(C6), 
        .C7(C7), 
        .anode(an) 
    );

    initial begin
        

        for (num = 0; num < 16; num = num + 1) begin
            #10;
            $display("num = %b | C1 = %b, C2 = %b, C3 = %b, C4 = %b, C5 = %b, C6 = %b, C7 = %b",
                      num, C1, C2, C3, C4, C5, C6, C7);
        end

        $finish;
    end
endmodule