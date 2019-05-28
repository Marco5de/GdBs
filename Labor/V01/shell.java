import static cTools.KernelWrapper.*;
import java.util.Scanner;
import java.io.File;
import java.util.Arrays;



class shell {
  public static void main(String[] args) {
	  
	String env = System.getenv("PATH");
	String[] path = env.split(":");
	
	 while(true){
		String[][][] input = splitInput();
		int pipe_count = 0;	
		int[] proc_fd = {STDIN_FILENO,STDOUT_FILENO};

		pipe_count = input.length-1;
		int new_stdin = 0;
		for(String[][] pipes : input){

			int[] pip_fd = new int[2];
			if(pipe_count > 0){
				if(pipe(pip_fd)<0)
					System.err.println("Pipe failed");
				pipe_count--;
			}
			System.err.println("Created Pipe: " + pip_fd[0] + " " + pip_fd[1] + " New stdin " + new_stdin);
			for(String[] comm : pipes){	
				/*
				for(String s : comm)
					System.err.println(s);
					*/
				return_valid_path(comm,path);	
				String[][] files = parse_redirect(comm);
					
				if(create_proc(files[0],files[1][0],files[2][0],pip_fd,new_stdin) != 0){
					break;
				}
			}
			new_stdin = pip_fd[1];
			pip_fd[0] = 0;
			pip_fd[1] = 0;
		}	
	}

  }	//return String[][] (param,fin,fout)
	static String[][] parse_redirect(String[] arr){
		int i;
		int index_out=0;
		int index_in=0;
		for(i=0; i<arr.length; i++){
			if(arr[i].equals(">"))
				index_out = i;
			else if(arr[i].equals("<"))
				index_in = i;
		}
		String[][] ret = new String[3][i];
		ret[1][0] = null;
		ret[2][0] = null;

		if(index_out!=0){
			ret[2][0] = arr[index_out+1];
		}
		if(index_in!=0){
			ret[1][0] = arr[index_in+1];	
		}

		if(index_out > index_in){
			if(index_in != 0)
				ret[0] = Arrays.copyOfRange(arr,0,index_in);
			else
				ret[0] = Arrays.copyOfRange(arr,0,index_out);
		}else if(index_out < index_in){
			if(index_out != 0)
				ret[0] = Arrays.copyOfRange(arr,0,index_out);
			else
				ret[0] = Arrays.copyOfRange(arr,0,index_in);
		}else 
			ret[0] = arr;
		
		return ret;
	}
  	static int redirect_output(String fout,String fin){
		//case redirect stdout
		if(fout != null){
			//close stdout and open file in arr[i+1]
			close(STDOUT_FILENO); // 1 = stdout
			int fd;
			if((fd=open(fout,O_WRONLY | O_CREAT))<0)
				return -1;
		}
		if(fin != null){
			close(STDIN_FILENO);
			int fd;
			if((fd=open(fin,O_RDONLY))<0)
				return -1;
		}
		return 0;
	}	

  	static void return_valid_path(String[] arr,String[] env){
		for(String s : env){
			File tmp = new File(s +"/"+ arr[0]);
			if(tmp.exists()){
				arr[0] = s +"/"+ arr[0];
				break;
			}
		}
	}

	static String[][][] splitInput() {
		Scanner in = new Scanner(System.in);
		System.out.print("Custom Shell: $ ");
		String input = in.nextLine();



		String[] pip_sep = input.split(" \\| ");


		String[][] sep_input = new String[pip_sep.length][]; 
		for(int i = 0; i<pip_sep.length; i++){
			sep_input[i] = pip_sep[i].split(" && ");
		}
		String[][][] split_input = new String[sep_input.length][][];  
		for(int i=0; i< sep_input.length; i++){
			split_input[i] = new String[sep_input[i].length][];
			for(int j=0; j<sep_input[i].length; j++) {
				split_input[i][j] = sep_input[i][j].split(" ");	
			}
		}
		
		for(String[][] d2 : split_input){
			for(String[] d1 : d2){
				System.err.println(Arrays.toString(d1));
			}
		}
		
		return split_input;
	}

	static int create_proc(String[] split_input,String fin, String fout,int[] pip_fd,int new_stdin){
		if(split_input[0].equals("exit")){
			System.err.println("\n");	
			exit(0);
		}

		int[] status = new int[1];
		int pid_child;


		if((pid_child = fork())<0){
			System.err.println("Error: fork");
			exit(1);
		}

		//unterscheidung zw. child und parent
		if(pid_child == 0){
			System.err.println("New stdin: " + new_stdin);
			if(new_stdin!=0){
				System.err.println("Setting new stdin in child");
				//dup2(STDIN_FILENO,pip_fd[0]);
				close(pip_fd[1]);
				dup2(pip_fd[0],STDIN_FILENO);

			}
			//code for child
			if(fout != null || fin != null){
				if(redirect_output(fout,fin)!=0){
					System.err.println("Error redirecting output");
					return 1;
				}
			}
			//code for child goes in here
			if(handle_pipes(pip_fd)<0){
				System.err.println("Error setting pipe out");
				return -1;
			}
			if(execv(split_input[0],split_input)<0){
				System.err.println("Error: execv");
				exit(1);
			}
		}
		else{
			//code for parent
			if(pip_fd[0] != 0){
				close(pip_fd[0]);
				close(pip_fd[1]);
			}
			if(waitpid(pid_child,status,0)<0){
				System.err.println("Error: waiting for child");
				exit(1);
			}
			if(status[0] != 0){
				System.err.println("Status: Child returned error code");
				return 1;
			}
		}	
	
		return 0;
	}

	static int handle_pipes(int[] fd){
		if(fd[0]==0 && fd[1]==0){
			System.err.println("Leaving handle pipes");
			return 0;
		}
		//fd[0] ist für das Lesen verantwortlich. Wird im folgenden nicht verwendet und daher geschlossen
		close(fd[0]);
		//es muss jetzt noch das Out des jetzigen Prozesses neu gesetzt werden
		dup2(fd[1],STDOUT_FILENO);
		System.err.println("Succesfully set new output fd");
		return 0;
		
	}
}
