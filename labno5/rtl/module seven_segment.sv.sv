module seven_segment (
    input logic [3:0] num,    
    input logic [2:0] sel,    
    output logic C1, C2, C3, C4, C5, C6, C7, 
    output logic [7:0] anode  
);

    logic [6:0] segments; 

    always_comb begin
        assign {C1, C2, C3, C4, C5, C6, C7} = segments;

    
        assign anode = 8'b11111110;
        case (num)
            4'b0000: segments = 7'b00000001; 
            4'b0001: segments = 7'b1001111; 
            4'b0010: segments = 7'b0010010; 
            4'b0011: segments = 7'b0000110; 
            4'b0100: segments = 7'b1001100; 
            4'b0101: segments = 7'b0100100; 
            4'b0110: segments = 7'b0100000; 
            4'b0111: segments = 7'b0001111; 
            4'b1000: segments = 7'b0000000; 
            4'b1001: segments = 7'b0000100; 
            4'b1010: segments = 7'b0001000; 
            4'b1011: segments = 7'b1100000; 
            4'b1100: segments = 7'b0110001; 
            4'b1101: segments = 7'b1000010; 
            4'b1110: segments = 7'b0110000; 
            4'b1111: segments = 7'b0111000;
            default: segments = 7'b1111111; 
        endcase
    end

endmodule





