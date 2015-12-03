%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Obtains all Chvatal Gomory Cuts  
% Author: Cynthia Wood
% [x_star, cuts, bCuts] = cutsFromSimplex(A , b, c)
% x_star is the solution given by simplex
% cuts are the lhs of the inequality
% bCuts rhs of inequality
% A is a matrix
% b,c are vectors to solve the following problem:
%   Min { c'x; Ax = b, x >= 0 }
% nn is the number of nodes
% bb is the number of maximal cliques
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

function [x_star, cuts, bCuts] = cutsFromSimplexForBilevel(A , b, c,nn,bb)



    tol = 1.e-10;
    [x_star,~,~,B_ind] = linprog2(A,b,c,0,inf,tol);
   
    %Check if the solution is integra1 (1 if integral, 0 otherwise)
    intgrl = isequal(x_star,floor(x_star));
    
    [m ,n] = size(A);
    

    
    if intgrl ==1
       disp('Integral solution');
    else 
       %Find the cuts associated with the nonintegral variables
       B = A(:, B_ind);
       cB = cond(B);
       
       if cB > 1.e+15
           disp('Condition number of optimal basis is too big :-('); 
       end
       
       %Let N_ind be the set of indices of nonbasic variables
       N_ind = setdiff(1:n, B_ind);
       
       %Let A_N be the submatrix of A with columns A_i, i in N
       A_N = A(:,N_ind);
       
       %From the optimal tableau we obtain the coefficients of
       %constraints x_B + Binv*A_N*x_N = Binv*b
       Binvb = B\b; 
       
       BinvA_N = B\A_N; 
        
       b_diff = Binvb-floor(Binvb); %
        
       bdiff_ind = find(b_diff > tol);
      
       % Create a Dictionary of all possible cuts
        
       bd_size = size(bdiff_ind);
       
       D = zeros(bd_size(1),n);
        
        
       for i = 1:bd_size(1)
        D(i, B_ind(bdiff_ind(i))) = 1;
        D(i, N_ind) = floor(BinvA_N(bdiff_ind(i),:));    
       end
       %D
       %spy(D)
       
       %This helps us transform our cuts in terms of slacks to the original
       %variables
       % T IS PROBLEM DEPENDENT
       T =[eye(3*nn+2*bb+1); -A(1:m,1:(3*nn+2*bb+1))];
       size(D)
       size(T)
       %T
       
       Cuts = D*T;
       
       figure;
       spy(Cuts);
       
       bC = D(:,n)*-1;
       
       bCuts = floor(Binvb(bdiff_ind)) -bC;
       
       x_ = sym('x_',[(3*nn+2*bb+1) 1]);
       
       cuts = sym(Cuts*x_);
       
       
    end

end