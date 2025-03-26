module register(
    input logic clk, reset, en,
    input logic [3:0] d,
    output logic [3:0] q
);

    always_ff @(posedge clk or posedge reset)
    begin
        if (reset)
            q <= 4'b0000;
        else if (en)
            q <= d;
    end
    
endmodule


module lab7 (
    input  logic clk,            
    input  logic reset,          
    input  logic write,          
    input  logic [3:0] num,      
    input  logic [2:0] sel,      
    output logic [6:0] seg,      
    output logic [7:0] an        
);

    logic clkout;  
    logic [2:0] sel_2x1;  
    logic reg_en [7:0]; 
    logic [3:0] q [7:0];
    logic [3:0] mux_8x1out;  
    logic [18:0] clk_counter;  
    logic [2:0] count;    

    
    register d_flip_flop[7:0] (
        .clk(clkout),
        .reset(reset),
        .en(reg_en),
        .d(num),
        .q(q)
    );

    
    always_comb begin
        case (sel_2x1)
            3'b000: mux_8x1out = q[0];
            3'b001: mux_8x1out = q[1];
            3'b010: mux_8x1out = q[2];
            3'b011: mux_8x1out = q[3];
            3'b100: mux_8x1out = q[4];
            3'b101: mux_8x1out = q[5];
            3'b110: mux_8x1out = q[6];
            3'b111: mux_8x1out = q[7];
            default: mux_8x1out = 4'b0000;
        endcase
    end

    always_comb begin
        case (sel_2x1)
            3'b000: an = 8'b11111110;
            3'b001: an = 8'b11111101;
            3'b010: an = 8'b11111011;
            3'b011: an = 8'b11110111;
            3'b100: an = 8'b11101111;
            3'b101: an = 8'b11011111;
            3'b110: an = 8'b10111111;
            3'b111: an = 8'b01111111;
            default: an = 8'b11111111;
        endcase
    end
    
    always_comb begin
        case (mux_8x1out)
            4'b0000: seg = 7'b0000001; 
            4'b0001: seg = 7'b1001111; 
            4'b0010: seg = 7'b0010010; 
            4'b0011: seg = 7'b0000110; 
            4'b0100: seg = 7'b1001100; 
            4'b0101: seg = 7'b0100100; 
            4'b0110: seg = 7'b0100000; 
            4'b0111: seg = 7'b0001111; 
            4'b1000: seg = 7'b0000000; 
            4'b1001: seg = 7'b0000100; 
            default: seg = 7'b1111111;
        endcase
    end


    
    always_comb begin
        sel_2x1 = (write) ? sel : count;
    end

    
    always_ff @(posedge clk or posedge reset) begin
        if (reset) begin
            clk_counter <= 0;
            clkout <= 0;
        end 
        else if (clk_counter == 499999) begin  
            clkout <= ~clkout;
            clk_counter <= 0;
        end 
        else begin
            clk_counter <= clk_counter + 1;
        end
    end

    always_ff @(posedge clkout or posedge reset) begin
        if (reset)
            count <= 0;
        else
            count <= count + 1'b1;
    end

    
    always_ff @(posedge clkout) begin
        for (int i = 0; i < 8; i = i + 1)
            reg_en[i] <= ~an[i] & write;
    end

endmodule
