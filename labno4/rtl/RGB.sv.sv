module RGB (
    input logic [1:0]a,
    input logic [1:0]b,  
    output logic r, g, bl   
);

    assign r = (~b[0]&~b[1])| (~b[0]&a[1]) | (a[0]&~a[1]&~b[1]) | (a[0]&a[1]) | (b[0]&a[1]&~b[1]);

   
    assign g = (~a[0] & ~a[1]) | (b[1] & ~a[0]) | (b[0] & b[1]) | (b[1] &a[0]& ~a[1])|(b[0] &a[0]& ~a[1]);

   
    assign bl = (~b[0] & a[0] & b[1]) | (~b[0] & a[0] & ~b[1])|(b[1] & a[0] & ~a[1])|(b[0]&~a[0])|(a[1] & ~b[1]);

endmodule