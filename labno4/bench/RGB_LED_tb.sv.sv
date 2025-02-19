
module RGB_LED_tb;
    
    logic [1:0] a, b;
    logic r, g, bl;

    RGB_LED uut (
        .a(a),
        .b(b),
        .r(r),
        .g(g),
        .bl(bl)
    );
    
   
    initial begin
        
       
       
        a = 2'b00; b = 2'b00; 
        #10;
        a = 2'b00; b = 2'b01; 
        #10;
        a = 2'b00; b = 2'b10; 
        #10;
        a = 2'b00; b = 2'b11; 
        #10;
        
        a = 2'b01; b = 2'b00; 
        #10;
        a = 2'b01; b = 2'b01; 
        #10;
        a = 2'b01; b = 2'b10; 
        #10;
        a = 2'b01; b = 2'b11; 
        #10;
        
        a = 2'b10; b = 2'b00; 
        #10;
        a = 2'b10; b = 2'b01; 
        #10;
        a = 2'b10; b = 2'b10; 
        #10;
        a = 2'b10; b = 2'b11; 
        #10;
        
        a = 2'b11; b = 2'b00; 
        #10;
        a = 2'b11; b = 2'b01; 
        #10;
        a = 2'b11; b = 2'b10; 
        #10;
        a = 2'b11; b = 2'b11; 
        #10;
        
        
       
        $finish;
    end
    initial begin
         $monitor(" a = %b,  b = %b, r = %b, g = %b, bl = %b",  a, b, r, g, bl);
    end             
endmodule
