module seven_segment (
    input  logic [3:0] num,     
    input  logic [2:0] sel,    
    output logic C1,
    output logic C2,
    output logic C3,
    output logic C4,
    output logic C5,
    output logic C6,
    output logic C7,
    output logic [7:0] anode   
);

    
    assign anode[0] = (sel[2] & sel[1] & sel[0]); 
    assign anode[1] = ~(~sel[2] & ~sel[1] &  sel[0]); 
    assign anode[2] = ~(~sel[2] &  sel[1] & ~sel[0]); 
    assign anode[3] = ~(~sel[2] &  sel[1] &  sel[0]); 
    assign anode[4] = ~( sel[2] & ~sel[1] & ~sel[0]); 
    assign anode[5] = ~( sel[2] & ~sel[1] &  sel[0]); 
    assign anode[6] = ~( sel[2] &  sel[1] & ~sel[0]); 
    assign anode[7] = ~( sel[2] &  sel[1] &  sel[0]); 

   
    assign C1 = (num[0] & ~num[1] & ~num[2] & ~num[3]) |(~num[0] & ~num[1] & num[2] & ~num[3]) | (num[0] & ~num[1] & num[2] & num[3]) | (num[0] & num[1] & ~num[2] & num[3]);
    assign C2 = (num[0] & ~num[1] & num[2] & ~num[3]) |  (~num[0] & num[1] & num[2]) |  (~num[0] &  num[2] & num[3]) |  (num[0] & num[1]  & num[3]);
    assign C3 = (~num[0] & num[1] & ~num[2] & ~num[3]) |  (~num[0] & num[2] & num[3]) |  ( num[1] & num[2] & num[3]);
    assign C4 = (num[0] & ~num[1] & ~num[2] & ~num[3]) |    (~num[0] & ~num[1] & num[2] & ~num[3]) |  (num[0] & num[1] & num[2]) |  (~num[0] & num[1] & ~num[2] & num[3]);
    assign C5 = (num[0]  & ~num[3]) |  (num[2] & ~num[3] & ~num[1]) |  (num[0] & ~num[1] & ~num[2]);
    assign C6 = (num[0] & ~num[1] & num[2] & num[3]) |  (num[0] &  ~num[2] & ~num[3]) |  (num[0] & num[1] & ~num[3]) |  ( num[1] & ~num[2] & ~num[3]);
    assign C7 = ( ~num[1] & ~num[2] & ~num[3]) |   (num[0] & num[1] & num[2] & ~num[3])|   (~num[0] & ~num[1] & num[2] & num[3]);


endmodule
