KernelWrapper:
	javac KernelWrapper.java
	(cd ..; javah -o cTools/KernelWrapper.h cTools.KernelWrapper)
	gcc -std=c99 -shared -fpic -I /opt/jdk/include -I /opt/jdk/include/linux -o libKernelWrapper.so KernelWrapper.c
	#gcc -std=c99 -shared -fpic -I /usr/lib/jvm/java-7-openjdk-amd64/include -I /usr/lib/jvm/java-7-openjdk-amd64/include/linux -o libKernelWrapper.so KernelWrapper.c

clean:
	/bin/rm -f *.class KernelWrapper.h libKernelWrapper.so
