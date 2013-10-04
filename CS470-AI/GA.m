%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Fun with Genetic Algorithms %
% Michael A. Goodrich         %
%                             %
% September 9, 2009           %
% CS 470 Example              %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
INITIAL_STATE=[1,1];
WORLD_DIM = 10;             % World dimension of rectangular world
MAX_EDGE_COST = 5;          % maximum edge cost in the world 


PATH_LEN = 2*WORLD_DIM;     % the maximum path length for each gene
NUM_GENES = 10;             % Don't let this get less than 10.
MAX_GENERATIONS = 60;       % Number of times that I try out new genes
MUTATION_THRESHOLD = 0.5;   % probability that a gene will mutate
NUM_TOP_GENES = min(floor(NUM_GENES/3),10);          % Number of top genes kept between generations
                            % Also, number of mutated top genes.
                            % Must be at least 3 but can't be more than
                            % floor(NUM_GENES/3)

%%%%%%%%%%%%%%%%
% Define World %
%%%%%%%%%%%%%%%%

x = [1:WORLD_DIM];  % Set of all x indices for nodes
y = [1:WORLD_DIM];  % Set of all y indices for nodes
E = zeros(WORLD_DIM, WORLD_DIM, WORLD_DIM, WORLD_DIM);
                    % Awkward representation of the edges 
                    % in the world.  Edges are connected
                    % only to their neighbors in x-y rect
                    % space
% Randomly specify edge costs and draw world.                   
figure(1);clf;
for (i=1:WORLD_DIM)   % For each x coordinate
    for (j=1:WORLD_DIM)   % For each y coordinate
        hold on;plot(i,j,'co');  % Plot a circle at each junction.
        
        % Horizontal lines
        if (i~=WORLD_DIM)
            tmp = 1+(MAX_EDGE_COST-1)*rand(1,1);
            E(i,i+1,j,j) = tmp;
            E(i+1,i,j,j) = tmp;  % makes computing fitness easier 
            % Set grayscale to inverse of cost, darker is cheaper
            c = 1-tmp/MAX_EDGE_COST;  
            set(line([i,i+1],[j,j]),'color',[c,c,c],'linewidth',5.5);
            %fprintf(1,'Edge cost from (%d,%d) to (%d,%d) = %.2f\n',...
            %    i, j, mod(i,WORLD_DIM)+1,j,...
            %    E(i,mod(i,WORLD_DIM)+1,j , j));
        end;
        % Vertical lines
        if (j~=WORLD_DIM)
            tmp = 1+(MAX_EDGE_COST-1)*rand(1,1);
            E(i,i,j,j+1) = tmp;
            E(i,i,j+1,j) = tmp;  % makes computing fitness easier
            c = 1-tmp/MAX_EDGE_COST;
            set(line([i,i],[j,j+1]),'color',[c,c,c],'linewidth',5.5);
            %fprintf(1,'Edge cost from (%d,%d) to (%d,%d) = %.2f\n',...
            %    i, j, i,mod(j,WORLD_DIM)+1,...
            %    E(i,i,j , mod(j,WORLD_DIM)+1));
        end;
    end
    %fprintf(1,'\n\n');
end


%%%%%%%%%%%%%%%%
% DEFINE GENES %
%%%%%%%%%%%%%%%%

% A gene is a sequence of NSEW movements.  Each location in
% the gene vector corresponds to a choice, so the first
% location corresponds to the choice of where to move from
% the starting cell, defined as (1,1).

% Initialize directions, 0=N, 1=E, 2=S, 3=W;
genes = floor(4*rand(NUM_GENES,PATH_LEN));
hbestpath=zeros(1,PATH_LEN);  % Handle for path segments

for (generation = 1: MAX_GENERATIONS)
    unfitness = zeros(NUM_GENES,1);  
    % The data structure for gene unfitness ("un" because it's a cost)
    % Compute fitness for each gene.
    for (gene_num=1:NUM_GENES)
        s=INITIAL_STATE;  % For each gene, reset to the start state.
        tmpE = E;  % Reinitialize edge cost.
        % I modify E so that repeated edges are bad, the objective
        % is to find the least cost path that never repeats an
        % edge.  Use tmpE as the temporary variable to boost costs
        % of repeated visits to edges. 
        for (path_step=1:PATH_LEN)
            % good coding style would offload this to a function, but viva
            % bad coding.

            % NORTH
            if (genes(gene_num,path_step) == 0)  % If going N
                if (s(1,1) == WORLD_DIM) % At top of world
                    s(1,1) = s(1,1); s(1,2) = s(1,2);  % don't move
                    unfitness(gene_num,1) = unfitness(gene_num,1) ...
                        + 20;  % hit a wall, penalize
                    %fprintf(1,'old_s=(%d,%d), new_s=(%d,%d), cost=%.2f\n',...
                    %    s(1,1),s(1,2),s(1,1),s(1,2), ...
                    %    20);
                else
                    unfitness(gene_num,1) = unfitness(gene_num,1) + ...
                        tmpE(s(1,1),s(1,1)+1,s(1,2),s(1,2)); 
                    %fprintf(1,'old_s=(%d,%d), new_s=(%d,%d), cost=%.2f\n',...
                    %    s(1,1),s(1,2),s(1,1)+1,s(1,2), ...
                    %    tmpE(s(1,1),s(1,1)+1,s(1,2),s(1,2)));
                    % Since I've visited this edge, make the cost high
                    tmpE(s(1,1),s(1,1)+1,s(1,2),s(1,2)) = 20;
                    tmpE(s(1,1)+1,s(1,1),s(1,2),s(1,2)) = 20;
                    % Update state.
                    s(1,1) = s(1,1)+1; s(1,2) = s(1,2); % go north
                end
            end
            % EAST
            if (genes(gene_num,path_step) == 1)  % If going E
                if (s(1,2) == WORLD_DIM) % At east side of world
                    s(1,1) = s(1,1); s(1,2) = s(1,2);  % don't move
                    unfitness(gene_num,1) = unfitness(gene_num,1) ...
                        + 20;  % hit a wall, penalize
                    %fprintf(1,'old_s=(%d,%d), new_s=(%d,%d), cost=%.2f\n',...
                    %    s(1,1),s(1,2),s(1,1),s(1,2), ...
                    %    20);
                else
                    unfitness(gene_num,1) = unfitness(gene_num,1) + ...
                        tmpE(s(1,1),s(1,1),s(1,2),s(1,2)+1); 
                    %fprintf(1,'old_s=(%d,%d), new_s=(%d,%d), cost=%.2f\n',...
                    %    s(1,1),s(1,2),s(1,1)+1,s(1,2), ...
                    %    tmpE(s(1,1),s(1,1),s(1,2),s(1,2)+1));
                    % Since I've visited this edge, make the cost high
                    tmpE(s(1,1),s(1,1),s(1,2),s(1,2)+1) = 20;
                    tmpE(s(1,1),s(1,1),s(1,2)+1,s(1,2)) = 20;
                    % Update state.
                    s(1,1) = s(1,1); s(1,2) = s(1,2)+1; % go east
                end;
            end        
                    % SOUTH
            if (genes(gene_num,path_step) == 2)  % If going S
                if (s(1,1) == 1) % At top of world
                    s(1,1) = s(1,1); s(1,2) = s(1,2);  % don't move
                    unfitness(gene_num,1) = unfitness(gene_num,1) ...
                        + 20;  % hit a wall, penalize
                    %fprintf(1,'old_s=(%d,%d), new_s=(%d,%d), cost=%.2f\n',...
                    %    s(1,1),s(1,2),s(1,1),s(1,2), ...
                    %    20);
                else
                    unfitness(gene_num,1) = unfitness(gene_num,1) + ...
                        tmpE(s(1,1),s(1,1)-1,s(1,2),s(1,2)); 
                    %fprintf(1,'old_s=(%d,%d), new_s=(%d,%d), cost=%.2f\n',...
                    %    s(1,1),s(1,2),s(1,1)-1,s(1,2), ...
                    %    tmpE(s(1,1),s(1,1)-1,s(1,2),s(1,2)));
                   % Since I've visited this edge, make the cost high
                    tmpE(s(1,1),s(1,1)-1,s(1,2),s(1,2)) = 20;
                    tmpE(s(1,1)-1,s(1,1),s(1,2),s(1,2)) = 20;
                    % Update state.
                    s(1,1) = s(1,1)-1; s(1,2) = s(1,2); % go south
                 end;
            end
            % WEST
            if (genes(gene_num,path_step) == 3)  % If going W
                if (s(1,2) == 1) % At east side of world
                    s(1,1) = s(1,1); s(1,2) = s(1,2);  % don't move
                    unfitness(gene_num,1) = unfitness(gene_num,1) ...
                        + 20;  % hit a wall, penalize
                    %fprintf(1,'old_s=(%d,%d), new_s=(%d,%d), cost=%.2f\n',...
                    %    s(1,1),s(1,2),s(1,1),s(1,2), ...
                    %    20);
                else
                    unfitness(gene_num,1) = unfitness(gene_num,1) + ...
                        tmpE(s(1,1),s(1,1),s(1,2),s(1,2)-1); 
                    %fprintf(1,'old_s=(%d,%d), new_s=(%d,%d), cost=%.2f\n',...
                    %    s(1,1),s(1,2),s(1,1)+1,s(1,2), ...
                    %    tmpE(s(1,1),s(1,1),s(1,2),s(1,2)-1));
                    % Since I've visited this edge, make the cost high
                    tmpE(s(1,1),s(1,1),s(1,2),s(1,2)-1) = 20;
                    tmpE(s(1,1),s(1,1),s(1,2)-1,s(1,2)) = 20;
                    % Update state.
                    s(1,1) = s(1,1); s(1,2) = s(1,2)-1; % go west
                end;
            end
         end
    end;

    % Look at the best and worst paths so far.
    [worst,argworst] = max(unfitness);
    [best,argbest] = min(unfitness);
    y_string = sprintf('Iteration %d',generation);
    set(ylabel(y_string),'fontsize',20);
    x_string = sprintf('Lowest cost so far = %.2f\n',best);  
    set(xlabel(x_string),'fontsize',20);
    s=INITIAL_STATE;  % For each gene, reset to the start state.

    for (path_step=1:PATH_LEN) 
        old_s = s;
        % NORTH
        if (genes(argbest,path_step) == 0)  % If going N
            if (s(1,1) == WORLD_DIM) % At top of world
                s(1,1) = s(1,1); s(1,2) = s(1,2);  % don't move
            else
               s(1,1) = s(1,1)+1; s(1,2) = s(1,2); % go north
            end
        end
        % EAST
        if (genes(argbest,path_step) == 1)  % If going E
            if (s(1,2) == WORLD_DIM) % At east side of world
                s(1,1) = s(1,1); s(1,2) = s(1,2);  % don't move
            else
                s(1,1) = s(1,1); s(1,2) = s(1,2)+1; % go east
            end;
        end        
        % SOUTH
        if (genes(argbest,path_step) == 2)  % If going S
            if (s(1,1) == 1) % At top of world
                s(1,1) = s(1,1); s(1,2) = s(1,2);  % don't move
            else
                s(1,1) = s(1,1)-1; s(1,2) = s(1,2); % go south
             end;
        end
        % WEST
        if (genes(argbest,path_step) == 3)  % If going W
            if (s(1,2) == 1) % At east side of world
                s(1,1) = s(1,1); s(1,2) = s(1,2);  % don't move
            else
               s(1,1) = s(1,1); s(1,2) = s(1,2)-1; % go west
            end;
        end
        %fprintf(1,'old_s=(%d,%d), new_s=(%d,%d)\n',olds(1,1),olds(1,2),s(1,1),s(1,2));
        if (hbestpath(path_step)==0)
            %hbestpath(path_step) = line(old_s,s);
            hbestpath(path_step) = line([old_s(1,2),s(1,2)],[old_s(1,1),s(1,1)]);
            set(hbestpath(path_step),'color','r','linewidth',3);
        else
            %set(hbestpath(path_step),'xdata',old_s,...
            %    'ydata',s);
            set(hbestpath(path_step),'xdata',[old_s(1,2),s(1,2)],...
                'ydata',[old_s(1,1),s(1,1)]);
        end

    end

    [ranked_unfitness, arg_rank_unfit] = sort(unfitness);  % puts the lowest cost genes at the top
    tmpgenes=floor(4*rand(NUM_GENES,PATH_LEN)); % new generation of genes
    
    % KEEP THE TOP GENES
    for (i=1:NUM_TOP_GENES)  % number of top genes kept each generation.
        tmpgenes(i,:) = genes(arg_rank_unfit(i),:);
    end
    % MUTATE THE TOP GENES
    for (i=NUM_TOP_GENES+1:2*NUM_TOP_GENES)  % number of top genes that have mutations
        tmpgenes(i,:) = genes(arg_rank_unfit(i-NUM_TOP_GENES),:);
        mutate = rand(1,PATH_LEN)>MUTATION_THRESHOLD;  % genes above threshold change
        tmpi = find(mutate==1);  % The index of the genes that will mutate
        if (sum(mutate)>0)  % Only mutate if there is at least one gene that needs to change
            tmpgenes(i,tmpi) = floor(4*rand(1,sum(mutate)));  % sum gives number of genes needing to change
        end;
    end
    % CROSSOVER THE TOP GENES
    % Choose the cross over point -- not first step or last
    cross_index = 1+floor((PATH_LEN-1) * rand(1,1));
    % New gene has top ranked gene's head and second ranked gene's tail
    tmpgenes(2*NUM_TOP_GENES+1,:) = tmpgenes(1,:); 
    tmpgenes(2*NUM_TOP_GENES+1,[cross_index:PATH_LEN]) = tmpgenes(2*NUM_TOP_GENES+2,[cross_index:PATH_LEN]);
    % New gene has second ranked gene's head and top ranked gene's tail
    tmpgenes(2*NUM_TOP_GENES+2,:) = tmpgenes(2,:); 
    tmpgenes(2*NUM_TOP_GENES+2,[cross_index:PATH_LEN]) = tmpgenes(2*NUM_TOP_GENES+1,[cross_index:PATH_LEN]);
    % New gene has top ranked gene's head and third ranked gene's tail
    tmpgenes(2*NUM_TOP_GENES+1,:) = tmpgenes(1,:); 
    tmpgenes(2*NUM_TOP_GENES+1,[cross_index:PATH_LEN]) = tmpgenes(2*NUM_TOP_GENES+3,[cross_index:PATH_LEN]);
    
    genes = tmpgenes;
    pause(.25)     
    
end  % End of number generations for loop