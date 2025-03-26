`timescale 1ns / 1ps

module lab7_tb;
    
    logic clk;
    logic reset;
    logic write;
    logic [2:0] sel;
    logic [3:0] num;
    logic [6:0] seg;
    logic [7:0] an;
    
    
    lab7 dut (
        .clk(clk),
        .reset(reset),
        .write(write),
        .sel(sel),
        .num(num),
        .seg(seg),
        .an(an)
    );
    
    always #5 clk = ~clk; 
    
    initial begin
        
        clk = 0;
        reset = 1;
        write = 0;
        sel = 0;
        num = 0;
        
        #20000000 reset = 0; 
        
       
        write = 1;
        sel = 3'b010;
        num = 4'b1010;
        #10000000;
        write = 0;
        
        
        repeat (5) #10000000;
        
        
        write = 1;
        sel = 3'b011;
        num = 4'b0001;
        #10000000;
        write = 0;
        
        repeat (5) #10000000;

        reset = 1;

        repeat (5) #10000000;

        reset = 0;

        repeat (5) #10000000;

        


        $stop;
    end
endmodule
