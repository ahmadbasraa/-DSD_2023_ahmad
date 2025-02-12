module task2(input a,b,c,
                 output x,y
                 );
       assign y = ((~(a&b))^(a|b))&(a|b);
       assign x =  (a|b)^(~c);
   
endmodule
