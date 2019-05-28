import static cTools.KernelWrapper.*;
import java.util.Arrays;
import java.util.ArrayList;
//Todo complete help printout
//ToDo if FIlename = "-" read from stdin
class head{

	public static void main(String[] args){
		int nbytes = 0;
		int nlines = 10;	

		boolean param_used = false;

		//check if there are enough input args supplied
		if(args.length<1){
			System.out.println("Not enough args");
			exit(0);
		}

		if(args[0].charAt(0) == '-' && args[0].length()>1){
			switch(args[0].charAt(1)){
				case 'c':
					nlines = Integer.parseInt(args[1]);
					if(nlines<0){System.err.println("Neg. number of lines");exit(1);}
					param_used = true;
					break;
				case 'n':
					nbytes = Integer.parseInt(args[1]);
					if(nbytes<0){System.err.println("Neg. number of bytes");exit(1);}
					param_used = true;
					break;	
				case '-':
					if(args[0].contains("help")){
						System.out.println("ToDo add help in here");
						exit(0);
					}break;	
			}
		}
		ArrayList<String> list = new ArrayList<>(Arrays.asList(args));

		if(param_used){
			//remove entries that arent files
			list.remove(0);
			list.remove(0);
		}
		
		//open one file at a time, read the specified amount and print it
		boolean header = list.size()>1;
		for(String s : list){
			if(header)
				System.out.println("===> " + s + " <===");
			int fd;
			if(s.equals("-")){
				fd = STDIN_FILENO;
			}else{
				if((fd = open(s,O_RDONLY))<0){
					System.err.println("Error opening File");
					exit(1);
				}
			}
			if(nbytes == 0){
				//print lines
				read_and_println(fd,nlines);
			}else{
				//print bytes
				read_printbytes(fd,nbytes);
			}
			System.out.println();
		}
		exit(0);
		
	}
	private static int read_and_println(int fd,int nlines){
		byte[] buffer = new byte[1];
		
		int res = 0;
		int len;
		int line_count = 0;
		String out = "";

		while(line_count < nlines){
			if((res = read(fd,buffer,1))<0)
				return -1;
		
			if(buffer[0]=='\n'){
				line_count++;	
			}	
			if(res == 0)
				break;
			out += (char)buffer[0];
		}
		System.out.println(out);
		return 0;
	}

	private static int read_printbytes(int fd, int nbytes){
		byte[] buffer = new byte[256];

		int total = 0;
		int res;
		String out = "";

		while(total < nbytes){
			if((res=read(fd,buffer,nbytes-total))<0)
				return -1;
			total += res;
			if(res==0)break;
			out += new String(buffer);
		}
		System.out.println(out);

		return 0;
	}
}

