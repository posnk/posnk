
../newkern/initrd/bin/sigtest:     file format elf32-i386


Disassembly of section .init:

08048074 <.init>:
 8048074:	e8 f3 00 00 00       	call   804816c <frame_dummy>
 8048079:	e8 92 95 00 00       	call   8051610 <__do_global_ctors_aux>

Disassembly of section .text:

08048080 <_start>:
 
## here you might want to get the argc/argv pairs somehow and then push
## them onto the stack...
 
# push envs
push $0x4020000
 8048080:	68 00 00 02 04       	push   $0x4020000
# push argv
push $0x4000000
 8048085:	68 00 00 00 04       	push   $0x4000000
# calculate argc
call strlistlen
 804808a:	e8 ad 02 00 00       	call   804833c <strlistlen>
# push argc
push %eax
 804808f:	50                   	push   %eax
# call user entry point
call main
 8048090:	e8 3b 01 00 00       	call   80481d0 <main>
 
# call the 'kill me' syscall to end
push %eax
 8048095:	50                   	push   %eax
call exit
 8048096:	e8 79 01 00 00       	call   8048214 <exit>

0804809b <lp>:
 
# loop in case we haven't yet rescheduled
lp:
hlt
 804809b:	f4                   	hlt    
jmp lp
 804809c:	eb fd                	jmp    804809b <lp>
 804809e:	90                   	nop
 804809f:	90                   	nop

080480a0 <deregister_tm_clones>:
 80480a0:	b8 6f 49 05 08       	mov    $0x805496f,%eax
 80480a5:	2d 6c 49 05 08       	sub    $0x805496c,%eax
 80480aa:	83 f8 06             	cmp    $0x6,%eax
 80480ad:	77 01                	ja     80480b0 <deregister_tm_clones+0x10>
 80480af:	c3                   	ret    
 80480b0:	b8 00 00 00 00       	mov    $0x0,%eax
 80480b5:	85 c0                	test   %eax,%eax
 80480b7:	74 f6                	je     80480af <deregister_tm_clones+0xf>
 80480b9:	55                   	push   %ebp
 80480ba:	89 e5                	mov    %esp,%ebp
 80480bc:	83 ec 18             	sub    $0x18,%esp
 80480bf:	c7 04 24 6c 49 05 08 	movl   $0x805496c,(%esp)
 80480c6:	ff d0                	call   *%eax
 80480c8:	c9                   	leave  
 80480c9:	c3                   	ret    
 80480ca:	66 90                	xchg   %ax,%ax

080480cc <register_tm_clones>:
 80480cc:	b8 6c 49 05 08       	mov    $0x805496c,%eax
 80480d1:	2d 6c 49 05 08       	sub    $0x805496c,%eax
 80480d6:	c1 f8 02             	sar    $0x2,%eax
 80480d9:	89 c2                	mov    %eax,%edx
 80480db:	c1 ea 1f             	shr    $0x1f,%edx
 80480de:	01 d0                	add    %edx,%eax
 80480e0:	d1 f8                	sar    %eax
 80480e2:	75 01                	jne    80480e5 <register_tm_clones+0x19>
 80480e4:	c3                   	ret    
 80480e5:	ba 00 00 00 00       	mov    $0x0,%edx
 80480ea:	85 d2                	test   %edx,%edx
 80480ec:	74 f6                	je     80480e4 <register_tm_clones+0x18>
 80480ee:	55                   	push   %ebp
 80480ef:	89 e5                	mov    %esp,%ebp
 80480f1:	83 ec 18             	sub    $0x18,%esp
 80480f4:	89 44 24 04          	mov    %eax,0x4(%esp)
 80480f8:	c7 04 24 6c 49 05 08 	movl   $0x805496c,(%esp)
 80480ff:	ff d2                	call   *%edx
 8048101:	c9                   	leave  
 8048102:	c3                   	ret    
 8048103:	90                   	nop

08048104 <__do_global_dtors_aux>:
 8048104:	80 3d 80 49 05 08 00 	cmpb   $0x0,0x8054980
 804810b:	75 5b                	jne    8048168 <__do_global_dtors_aux+0x64>
 804810d:	55                   	push   %ebp
 804810e:	89 e5                	mov    %esp,%ebp
 8048110:	53                   	push   %ebx
 8048111:	83 ec 14             	sub    $0x14,%esp
 8048114:	a1 84 49 05 08       	mov    0x8054984,%eax
 8048119:	bb 0c 40 05 08       	mov    $0x805400c,%ebx
 804811e:	81 eb 08 40 05 08    	sub    $0x8054008,%ebx
 8048124:	c1 fb 02             	sar    $0x2,%ebx
 8048127:	4b                   	dec    %ebx
 8048128:	39 d8                	cmp    %ebx,%eax
 804812a:	73 16                	jae    8048142 <__do_global_dtors_aux+0x3e>
 804812c:	40                   	inc    %eax
 804812d:	a3 84 49 05 08       	mov    %eax,0x8054984
 8048132:	ff 14 85 08 40 05 08 	call   *0x8054008(,%eax,4)
 8048139:	a1 84 49 05 08       	mov    0x8054984,%eax
 804813e:	39 d8                	cmp    %ebx,%eax
 8048140:	72 ea                	jb     804812c <__do_global_dtors_aux+0x28>
 8048142:	e8 59 ff ff ff       	call   80480a0 <deregister_tm_clones>
 8048147:	b8 00 00 00 00       	mov    $0x0,%eax
 804814c:	85 c0                	test   %eax,%eax
 804814e:	74 0c                	je     804815c <__do_global_dtors_aux+0x58>
 8048150:	c7 04 24 78 1b 05 08 	movl   $0x8051b78,(%esp)
 8048157:	e8 a4 7e fb f7       	call   0 <_start-0x8048080>
 804815c:	c6 05 80 49 05 08 01 	movb   $0x1,0x8054980
 8048163:	83 c4 14             	add    $0x14,%esp
 8048166:	5b                   	pop    %ebx
 8048167:	5d                   	pop    %ebp
 8048168:	c3                   	ret    
 8048169:	8d 76 00             	lea    0x0(%esi),%esi

0804816c <frame_dummy>:
 804816c:	55                   	push   %ebp
 804816d:	89 e5                	mov    %esp,%ebp
 804816f:	83 ec 18             	sub    $0x18,%esp
 8048172:	b8 00 00 00 00       	mov    $0x0,%eax
 8048177:	85 c0                	test   %eax,%eax
 8048179:	74 14                	je     804818f <frame_dummy+0x23>
 804817b:	c7 44 24 04 88 49 05 	movl   $0x8054988,0x4(%esp)
 8048182:	08 
 8048183:	c7 04 24 78 1b 05 08 	movl   $0x8051b78,(%esp)
 804818a:	e8 71 7e fb f7       	call   0 <_start-0x8048080>
 804818f:	a1 10 40 05 08       	mov    0x8054010,%eax
 8048194:	85 c0                	test   %eax,%eax
 8048196:	74 12                	je     80481aa <frame_dummy+0x3e>
 8048198:	b8 00 00 00 00       	mov    $0x0,%eax
 804819d:	85 c0                	test   %eax,%eax
 804819f:	74 09                	je     80481aa <frame_dummy+0x3e>
 80481a1:	c7 04 24 10 40 05 08 	movl   $0x8054010,(%esp)
 80481a8:	ff d0                	call   *%eax
 80481aa:	c9                   	leave  
 80481ab:	e9 1c ff ff ff       	jmp    80480cc <register_tm_clones>

080481b0 <our_handler>:
 80481b0:	55                   	push   %ebp
 80481b1:	89 e5                	mov    %esp,%ebp
 80481b3:	83 ec 18             	sub    $0x18,%esp
 80481b6:	8b 45 08             	mov    0x8(%ebp),%eax
 80481b9:	89 44 24 04          	mov    %eax,0x4(%esp)
 80481bd:	c7 04 24 40 16 05 08 	movl   $0x8051640,(%esp)
 80481c4:	e8 ab 00 00 00       	call   8048274 <printf>
 80481c9:	e8 3e 07 00 00       	call   804890c <exitsig>
 80481ce:	c9                   	leave  
 80481cf:	c3                   	ret    

080481d0 <main>:
 80481d0:	55                   	push   %ebp
 80481d1:	89 e5                	mov    %esp,%ebp
 80481d3:	83 e4 f0             	and    $0xfffffff0,%esp
 80481d6:	83 ec 20             	sub    $0x20,%esp
 80481d9:	c7 04 24 5b 16 05 08 	movl   $0x805165b,(%esp)
 80481e0:	e8 3b 01 00 00       	call   8048320 <puts>
 80481e5:	c7 44 24 04 b0 81 04 	movl   $0x80481b0,0x4(%esp)
 80481ec:	08 
 80481ed:	c7 04 24 1e 00 00 00 	movl   $0x1e,(%esp)
 80481f4:	e8 bf 06 00 00       	call   80488b8 <signal>
 80481f9:	c7 04 24 71 16 05 08 	movl   $0x8051671,(%esp)
 8048200:	e8 1b 01 00 00       	call   8048320 <puts>
 8048205:	8d 44 24 1c          	lea    0x1c(%esp),%eax
 8048209:	89 04 24             	mov    %eax,(%esp)
 804820c:	e8 87 09 00 00       	call   8048b98 <wait>
 8048211:	eb f2                	jmp    8048205 <main+0x35>
 8048213:	90                   	nop

08048214 <exit>:
 */

void
_DEFUN (exit, (code),
	int code)
{
 8048214:	53                   	push   %ebx
 8048215:	83 ec 18             	sub    $0x18,%esp
 8048218:	8b 5c 24 20          	mov    0x20(%esp),%ebx
#ifdef _LITE_EXIT
  /* Refer to comments in __atexit.c for more details of lite exit.  */
  void __call_exitprocs _PARAMS ((int, _PTR)) __attribute__((weak));
  if (__call_exitprocs)
#endif
    __call_exitprocs (code, NULL);
 804821c:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
 8048223:	00 
 8048224:	89 1c 24             	mov    %ebx,(%esp)
 8048227:	e8 b4 2c 00 00       	call   804aee0 <__call_exitprocs>

  if (_GLOBAL_REENT->__cleanup)
 804822c:	8b 15 8c 16 05 08    	mov    0x805168c,%edx
 8048232:	8b 42 3c             	mov    0x3c(%edx),%eax
 8048235:	85 c0                	test   %eax,%eax
 8048237:	74 05                	je     804823e <exit+0x2a>
    (*_GLOBAL_REENT->__cleanup) (_GLOBAL_REENT);
 8048239:	89 14 24             	mov    %edx,(%esp)
 804823c:	ff d0                	call   *%eax
  _exit (code);
 804823e:	89 1c 24             	mov    %ebx,(%esp)
 8048241:	e8 3a 02 00 00       	call   8048480 <_exit>
 8048246:	90                   	nop
 8048247:	90                   	nop

08048248 <_printf_r>:

int
_DEFUN(_printf_r, (ptr, fmt),
       struct _reent *ptr _AND
       const char *__restrict fmt _DOTS)
{
 8048248:	83 ec 1c             	sub    $0x1c,%esp
 804824b:	8b 44 24 20          	mov    0x20(%esp),%eax
  int ret;
  va_list ap;

  _REENT_SMALL_CHECK_INIT (ptr);
  va_start (ap, fmt);
 804824f:	8d 54 24 28          	lea    0x28(%esp),%edx
  ret = _vfprintf_r (ptr, _stdout_r (ptr), fmt, ap);
 8048253:	89 54 24 0c          	mov    %edx,0xc(%esp)
 8048257:	8b 54 24 24          	mov    0x24(%esp),%edx
 804825b:	89 54 24 08          	mov    %edx,0x8(%esp)
 804825f:	8b 50 08             	mov    0x8(%eax),%edx
 8048262:	89 54 24 04          	mov    %edx,0x4(%esp)
 8048266:	89 04 24             	mov    %eax,(%esp)
 8048269:	e8 fe 09 00 00       	call   8048c6c <_vfprintf_r>
  va_end (ap);
  return ret;
}
 804826e:	83 c4 1c             	add    $0x1c,%esp
 8048271:	c3                   	ret    
 8048272:	66 90                	xchg   %ax,%ax

08048274 <printf>:
#ifndef _REENT_ONLY

int
_DEFUN(printf, (fmt),
       const char *__restrict fmt _DOTS)
{
 8048274:	83 ec 1c             	sub    $0x1c,%esp
  int ret;
  va_list ap;
  struct _reent *ptr = _REENT;
 8048277:	a1 40 40 05 08       	mov    0x8054040,%eax

  _REENT_SMALL_CHECK_INIT (ptr);
  va_start (ap, fmt);
 804827c:	8d 54 24 24          	lea    0x24(%esp),%edx
  ret = _vfprintf_r (ptr, _stdout_r (ptr), fmt, ap);
 8048280:	89 54 24 0c          	mov    %edx,0xc(%esp)
 8048284:	8b 54 24 20          	mov    0x20(%esp),%edx
 8048288:	89 54 24 08          	mov    %edx,0x8(%esp)
 804828c:	8b 50 08             	mov    0x8(%eax),%edx
 804828f:	89 54 24 04          	mov    %edx,0x4(%esp)
 8048293:	89 04 24             	mov    %eax,(%esp)
 8048296:	e8 d1 09 00 00       	call   8048c6c <_vfprintf_r>
  va_end (ap);
  return ret;
}
 804829b:	83 c4 1c             	add    $0x1c,%esp
 804829e:	c3                   	ret    
 804829f:	90                   	nop

080482a0 <_puts_r>:

int
_DEFUN(_puts_r, (ptr, s),
       struct _reent *ptr _AND
       _CONST char * s)
{
 80482a0:	56                   	push   %esi
 80482a1:	53                   	push   %ebx
 80482a2:	83 ec 34             	sub    $0x34,%esp
 80482a5:	8b 5c 24 40          	mov    0x40(%esp),%ebx
 80482a9:	8b 74 24 44          	mov    0x44(%esp),%esi
#ifdef _FVWRITE_IN_STREAMIO
  int result;
  size_t c = strlen (s);
 80482ad:	89 34 24             	mov    %esi,(%esp)
 80482b0:	e8 3b 09 00 00       	call   8048bf0 <strlen>
  struct __suio uio;
  struct __siov iov[2];
  FILE *fp;

  iov[0].iov_base = s;
 80482b5:	89 74 24 20          	mov    %esi,0x20(%esp)
  iov[0].iov_len = c;
 80482b9:	89 44 24 24          	mov    %eax,0x24(%esp)
  iov[1].iov_base = "\n";
 80482bd:	c7 44 24 28 92 16 05 	movl   $0x8051692,0x28(%esp)
 80482c4:	08 
  iov[1].iov_len = 1;
 80482c5:	c7 44 24 2c 01 00 00 	movl   $0x1,0x2c(%esp)
 80482cc:	00 
  uio.uio_resid = c + 1;
 80482cd:	40                   	inc    %eax
 80482ce:	89 44 24 1c          	mov    %eax,0x1c(%esp)
  uio.uio_iov = &iov[0];
 80482d2:	8d 44 24 20          	lea    0x20(%esp),%eax
 80482d6:	89 44 24 14          	mov    %eax,0x14(%esp)
  uio.uio_iovcnt = 2;
 80482da:	c7 44 24 18 02 00 00 	movl   $0x2,0x18(%esp)
 80482e1:	00 

  _REENT_SMALL_CHECK_INIT (ptr);
  fp = _stdout_r (ptr);
 80482e2:	8b 43 08             	mov    0x8(%ebx),%eax
  _newlib_flockfile_start (fp);
  ORIENT (fp, -1);
 80482e5:	8b 50 0c             	mov    0xc(%eax),%edx
 80482e8:	f6 c6 20             	test   $0x20,%dh
 80482eb:	75 0e                	jne    80482fb <_puts_r+0x5b>
 80482ed:	80 ce 20             	or     $0x20,%dh
 80482f0:	66 89 50 0c          	mov    %dx,0xc(%eax)
 80482f4:	81 60 64 ff df ff ff 	andl   $0xffffdfff,0x64(%eax)
  result = (__sfvwrite_r (ptr, fp, &uio) ? EOF : '\n');
 80482fb:	8d 54 24 14          	lea    0x14(%esp),%edx
 80482ff:	89 54 24 08          	mov    %edx,0x8(%esp)
 8048303:	89 44 24 04          	mov    %eax,0x4(%esp)
 8048307:	89 1c 24             	mov    %ebx,(%esp)
 804830a:	e8 49 4a 00 00       	call   804cd58 <__sfvwrite_r>
 804830f:	83 f8 01             	cmp    $0x1,%eax
 8048312:	19 c0                	sbb    %eax,%eax
 8048314:	83 e0 0b             	and    $0xb,%eax
 8048317:	48                   	dec    %eax

err:
  _newlib_flockfile_end (fp);
  return result;
#endif
}
 8048318:	83 c4 34             	add    $0x34,%esp
 804831b:	5b                   	pop    %ebx
 804831c:	5e                   	pop    %esi
 804831d:	c3                   	ret    
 804831e:	66 90                	xchg   %ax,%ax

08048320 <puts>:
#ifndef _REENT_ONLY

int
_DEFUN(puts, (s),
       char _CONST * s)
{
 8048320:	83 ec 1c             	sub    $0x1c,%esp
  return _puts_r (_REENT, s);
 8048323:	8b 44 24 20          	mov    0x20(%esp),%eax
 8048327:	89 44 24 04          	mov    %eax,0x4(%esp)
 804832b:	a1 40 40 05 08       	mov    0x8054040,%eax
 8048330:	89 04 24             	mov    %eax,(%esp)
 8048333:	e8 68 ff ff ff       	call   80482a0 <_puts_r>
}
 8048338:	83 c4 1c             	add    $0x1c,%esp
 804833b:	c3                   	ret    

0804833c <strlistlen>:
 */

char **environ = (char **) 0x4020000; 

int strlistlen(char **list)
{
 804833c:	8b 4c 24 04          	mov    0x4(%esp),%ecx
	int len = 0;
 8048340:	31 c0                	xor    %eax,%eax
	while (list[len++]);
 8048342:	eb 02                	jmp    8048346 <strlistlen+0xa>
 8048344:	89 d0                	mov    %edx,%eax
 8048346:	8d 50 01             	lea    0x1(%eax),%edx
 8048349:	83 3c 81 00          	cmpl   $0x0,(%ecx,%eax,4)
 804834d:	75 f5                	jne    8048344 <strlistlen+0x8>
	return len-1;
}
 804834f:	c3                   	ret    

08048350 <_execve>:
int
_DEFUN (_execve, (name, argv, env),
        char  *name  _AND
        char **argv  _AND
        char **env)
{
 8048350:	55                   	push   %ebp
 8048351:	57                   	push   %edi
 8048352:	56                   	push   %esi
 8048353:	53                   	push   %ebx
 8048354:	83 ec 5c             	sub    $0x5c,%esp
 8048357:	8b 6c 24 74          	mov    0x74(%esp),%ebp
 804835b:	8b 7c 24 78          	mov    0x78(%esp),%edi

char **environ = (char **) 0x4020000; 

int strlistlen(char **list)
{
	int len = 0;
 804835f:	31 db                	xor    %ebx,%ebx
 8048361:	8d 76 00             	lea    0x0(%esi),%esi
	while (list[len++]);
 8048364:	8b 44 9d 00          	mov    0x0(%ebp,%ebx,4),%eax
 8048368:	43                   	inc    %ebx
 8048369:	85 c0                	test   %eax,%eax
 804836b:	75 f7                	jne    8048364 <_execve+0x14>

char **environ = (char **) 0x4020000; 

int strlistlen(char **list)
{
	int len = 0;
 804836d:	c7 44 24 24 00 00 00 	movl   $0x0,0x24(%esp)
 8048374:	00 
 8048375:	8d 76 00             	lea    0x0(%esi),%esi
	while (list[len++]);
 8048378:	8b 14 87             	mov    (%edi,%eax,4),%edx
 804837b:	40                   	inc    %eax
 804837c:	85 d2                	test   %edx,%edx
 804837e:	75 f8                	jne    8048378 <_execve+0x28>
 8048380:	89 44 24 24          	mov    %eax,0x24(%esp)
        char **env)
{
   int c;
   uint32_t argvc = (uint32_t) (strlistlen(argv) + 1);
   uint32_t envc = (uint32_t) (strlistlen(env) + 1);
   uintptr_t *argvs = malloc(argvc * sizeof(char *));
 8048384:	8d 14 9d 00 00 00 00 	lea    0x0(,%ebx,4),%edx
 804838b:	89 14 24             	mov    %edx,(%esp)
 804838e:	89 54 24 18          	mov    %edx,0x18(%esp)
 8048392:	e8 ed 50 00 00       	call   804d484 <malloc>
 8048397:	89 c6                	mov    %eax,%esi
   uintptr_t *envs = malloc(envc * sizeof(char *));
 8048399:	8b 4c 24 24          	mov    0x24(%esp),%ecx
 804839d:	c1 e1 02             	shl    $0x2,%ecx
 80483a0:	89 0c 24             	mov    %ecx,(%esp)
 80483a3:	89 4c 24 1c          	mov    %ecx,0x1c(%esp)
 80483a7:	e8 d8 50 00 00       	call   804d484 <malloc>
 80483ac:	89 44 24 28          	mov    %eax,0x28(%esp)
   uint32_t a[] = {(uint32_t) name, (uint32_t) argv, (uint32_t) env, (uint32_t) argvs};
 80483b0:	8b 44 24 70          	mov    0x70(%esp),%eax
 80483b4:	89 44 24 30          	mov    %eax,0x30(%esp)
 80483b8:	89 6c 24 34          	mov    %ebp,0x34(%esp)
 80483bc:	89 7c 24 38          	mov    %edi,0x38(%esp)
 80483c0:	89 74 24 3c          	mov    %esi,0x3c(%esp)
   uint32_t b[] = {(uint32_t) 1+strlen(name), argvc * sizeof(char *), envc * sizeof(char *) , (uint32_t) envs};
 80483c4:	89 04 24             	mov    %eax,(%esp)
 80483c7:	e8 24 08 00 00       	call   8048bf0 <strlen>
 80483cc:	40                   	inc    %eax
 80483cd:	89 44 24 40          	mov    %eax,0x40(%esp)
 80483d1:	8b 54 24 18          	mov    0x18(%esp),%edx
 80483d5:	89 54 24 44          	mov    %edx,0x44(%esp)
 80483d9:	8b 4c 24 1c          	mov    0x1c(%esp),%ecx
 80483dd:	89 4c 24 48          	mov    %ecx,0x48(%esp)
 80483e1:	8b 44 24 28          	mov    0x28(%esp),%eax
 80483e5:	89 44 24 4c          	mov    %eax,0x4c(%esp)
   for (c = 0; c < (argvc - 1); c++)
 80483e9:	83 fb 01             	cmp    $0x1,%ebx
 80483ec:	74 34                	je     8048422 <_execve+0xd2>
}

int strlistlen(char **list);

int
_DEFUN (_execve, (name, argv, env),
 80483ee:	4b                   	dec    %ebx
 80483ef:	89 5c 24 20          	mov    %ebx,0x20(%esp)
   uint32_t envc = (uint32_t) (strlistlen(env) + 1);
   uintptr_t *argvs = malloc(argvc * sizeof(char *));
   uintptr_t *envs = malloc(envc * sizeof(char *));
   uint32_t a[] = {(uint32_t) name, (uint32_t) argv, (uint32_t) env, (uint32_t) argvs};
   uint32_t b[] = {(uint32_t) 1+strlen(name), argvc * sizeof(char *), envc * sizeof(char *) , (uint32_t) envs};
   for (c = 0; c < (argvc - 1); c++)
 80483f3:	31 c0                	xor    %eax,%eax
 80483f5:	31 db                	xor    %ebx,%ebx
 80483f7:	89 7c 24 2c          	mov    %edi,0x2c(%esp)
 80483fb:	89 ef                	mov    %ebp,%edi
 80483fd:	8d 76 00             	lea    0x0(%esi),%esi
	argvs[c] = strlen(argv[c]) + 1;
 8048400:	c1 e0 02             	shl    $0x2,%eax
 8048403:	8d 2c 06             	lea    (%esi,%eax,1),%ebp
 8048406:	8b 04 07             	mov    (%edi,%eax,1),%eax
 8048409:	89 04 24             	mov    %eax,(%esp)
 804840c:	e8 df 07 00 00       	call   8048bf0 <strlen>
 8048411:	40                   	inc    %eax
 8048412:	89 45 00             	mov    %eax,0x0(%ebp)
   uint32_t envc = (uint32_t) (strlistlen(env) + 1);
   uintptr_t *argvs = malloc(argvc * sizeof(char *));
   uintptr_t *envs = malloc(envc * sizeof(char *));
   uint32_t a[] = {(uint32_t) name, (uint32_t) argv, (uint32_t) env, (uint32_t) argvs};
   uint32_t b[] = {(uint32_t) 1+strlen(name), argvc * sizeof(char *), envc * sizeof(char *) , (uint32_t) envs};
   for (c = 0; c < (argvc - 1); c++)
 8048415:	43                   	inc    %ebx
 8048416:	89 d8                	mov    %ebx,%eax
 8048418:	3b 5c 24 20          	cmp    0x20(%esp),%ebx
 804841c:	75 e2                	jne    8048400 <_execve+0xb0>
 804841e:	8b 7c 24 2c          	mov    0x2c(%esp),%edi
	argvs[c] = strlen(argv[c]) + 1;
   for (c = 0; c < (envc - 1); c++)
 8048422:	83 7c 24 24 01       	cmpl   $0x1,0x24(%esp)
 8048427:	74 31                	je     804845a <_execve+0x10a>
}

int strlistlen(char **list);

int
_DEFUN (_execve, (name, argv, env),
 8048429:	8b 74 24 24          	mov    0x24(%esp),%esi
 804842d:	4e                   	dec    %esi
   uintptr_t *envs = malloc(envc * sizeof(char *));
   uint32_t a[] = {(uint32_t) name, (uint32_t) argv, (uint32_t) env, (uint32_t) argvs};
   uint32_t b[] = {(uint32_t) 1+strlen(name), argvc * sizeof(char *), envc * sizeof(char *) , (uint32_t) envs};
   for (c = 0; c < (argvc - 1); c++)
	argvs[c] = strlen(argv[c]) + 1;
   for (c = 0; c < (envc - 1); c++)
 804842e:	31 c0                	xor    %eax,%eax
}

int strlistlen(char **list);

int
_DEFUN (_execve, (name, argv, env),
 8048430:	31 db                	xor    %ebx,%ebx
 8048432:	8b 6c 24 28          	mov    0x28(%esp),%ebp
 8048436:	89 74 24 20          	mov    %esi,0x20(%esp)
 804843a:	89 fe                	mov    %edi,%esi
   uint32_t a[] = {(uint32_t) name, (uint32_t) argv, (uint32_t) env, (uint32_t) argvs};
   uint32_t b[] = {(uint32_t) 1+strlen(name), argvc * sizeof(char *), envc * sizeof(char *) , (uint32_t) envs};
   for (c = 0; c < (argvc - 1); c++)
	argvs[c] = strlen(argv[c]) + 1;
   for (c = 0; c < (envc - 1); c++)
	envs[c] = strlen(env[c]) + 1;
 804843c:	c1 e0 02             	shl    $0x2,%eax
 804843f:	8d 7c 05 00          	lea    0x0(%ebp,%eax,1),%edi
 8048443:	8b 04 06             	mov    (%esi,%eax,1),%eax
 8048446:	89 04 24             	mov    %eax,(%esp)
 8048449:	e8 a2 07 00 00       	call   8048bf0 <strlen>
 804844e:	40                   	inc    %eax
 804844f:	89 07                	mov    %eax,(%edi)
   uintptr_t *envs = malloc(envc * sizeof(char *));
   uint32_t a[] = {(uint32_t) name, (uint32_t) argv, (uint32_t) env, (uint32_t) argvs};
   uint32_t b[] = {(uint32_t) 1+strlen(name), argvc * sizeof(char *), envc * sizeof(char *) , (uint32_t) envs};
   for (c = 0; c < (argvc - 1); c++)
	argvs[c] = strlen(argv[c]) + 1;
   for (c = 0; c < (envc - 1); c++)
 8048451:	43                   	inc    %ebx
 8048452:	89 d8                	mov    %ebx,%eax
 8048454:	3b 5c 24 20          	cmp    0x20(%esp),%ebx
 8048458:	75 e2                	jne    804843c <_execve+0xec>
	envs[c] = strlen(env[c]) + 1;

   return (int) nk_do_syscall(SYS_EXECVE, a, b); 
 804845a:	8d 44 24 40          	lea    0x40(%esp),%eax
 804845e:	89 44 24 08          	mov    %eax,0x8(%esp)
 8048462:	8d 44 24 30          	lea    0x30(%esp),%eax
 8048466:	89 44 24 04          	mov    %eax,0x4(%esp)
 804846a:	c7 04 24 23 00 00 00 	movl   $0x23,(%esp)
 8048471:	e8 22 66 00 00       	call   804ea98 <nk_do_syscall>
}
 8048476:	83 c4 5c             	add    $0x5c,%esp
 8048479:	5b                   	pop    %ebx
 804847a:	5e                   	pop    %esi
 804847b:	5f                   	pop    %edi
 804847c:	5d                   	pop    %ebp
 804847d:	c3                   	ret    
 804847e:	66 90                	xchg   %ax,%ax

08048480 <_exit>:

_VOID
_DEFUN (_exit, (rc),
	int rc)
{
 8048480:	57                   	push   %edi
 8048481:	83 ec 38             	sub    $0x38,%esp
   int a[] = {(uint32_t) rc,0,0,0};
 8048484:	8b 44 24 40          	mov    0x40(%esp),%eax
 8048488:	89 44 24 10          	mov    %eax,0x10(%esp)
 804848c:	c7 44 24 14 00 00 00 	movl   $0x0,0x14(%esp)
 8048493:	00 
 8048494:	c7 44 24 18 00 00 00 	movl   $0x0,0x18(%esp)
 804849b:	00 
 804849c:	c7 44 24 1c 00 00 00 	movl   $0x0,0x1c(%esp)
 80484a3:	00 
   int b[] = {0,0,0,0};
 80484a4:	8d 7c 24 20          	lea    0x20(%esp),%edi
 80484a8:	b9 10 00 00 00       	mov    $0x10,%ecx
 80484ad:	31 c0                	xor    %eax,%eax
 80484af:	f3 aa                	rep stos %al,%es:(%edi)
   nk_do_syscall(SYS_EXIT, a, b); 
 80484b1:	8d 44 24 20          	lea    0x20(%esp),%eax
 80484b5:	89 44 24 08          	mov    %eax,0x8(%esp)
 80484b9:	8d 44 24 10          	lea    0x10(%esp),%eax
 80484bd:	89 44 24 04          	mov    %eax,0x4(%esp)
 80484c1:	c7 04 24 06 00 00 00 	movl   $0x6,(%esp)
 80484c8:	e8 cb 65 00 00       	call   804ea98 <nk_do_syscall>
}
 80484cd:	83 c4 38             	add    $0x38,%esp
 80484d0:	5f                   	pop    %edi
 80484d1:	c3                   	ret    
 80484d2:	66 90                	xchg   %ax,%ax

080484d4 <fork>:

int
_DEFUN (fork, (),
        _NOARGS)
{
 80484d4:	57                   	push   %edi
 80484d5:	83 ec 38             	sub    $0x38,%esp
   int a[] = {0,0,0,0};
 80484d8:	8d 7c 24 10          	lea    0x10(%esp),%edi
 80484dc:	31 c0                	xor    %eax,%eax
 80484de:	b9 10 00 00 00       	mov    $0x10,%ecx
 80484e3:	f3 aa                	rep stos %al,%es:(%edi)
   int b[] = {0,0,0,0};
 80484e5:	8d 7c 24 20          	lea    0x20(%esp),%edi
 80484e9:	b1 10                	mov    $0x10,%cl
 80484eb:	f3 aa                	rep stos %al,%es:(%edi)
   return (int)nk_do_syscall(SYS_FORK, a, b); 
 80484ed:	8d 44 24 20          	lea    0x20(%esp),%eax
 80484f1:	89 44 24 08          	mov    %eax,0x8(%esp)
 80484f5:	8d 44 24 10          	lea    0x10(%esp),%eax
 80484f9:	89 44 24 04          	mov    %eax,0x4(%esp)
 80484fd:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
 8048504:	e8 8f 65 00 00       	call   804ea98 <nk_do_syscall>
}
 8048509:	83 c4 38             	add    $0x38,%esp
 804850c:	5f                   	pop    %edi
 804850d:	c3                   	ret    
 804850e:	66 90                	xchg   %ax,%ax

08048510 <vfork>:

int
_DEFUN (vfork, (),
        _NOARGS)
{
 8048510:	57                   	push   %edi
 8048511:	83 ec 38             	sub    $0x38,%esp
   int a[] = {0,0,0,0};
 8048514:	8d 7c 24 10          	lea    0x10(%esp),%edi
 8048518:	31 c0                	xor    %eax,%eax
 804851a:	b9 10 00 00 00       	mov    $0x10,%ecx
 804851f:	f3 aa                	rep stos %al,%es:(%edi)
   int b[] = {0,0,0,0};
 8048521:	8d 7c 24 20          	lea    0x20(%esp),%edi
 8048525:	b1 10                	mov    $0x10,%cl
 8048527:	f3 aa                	rep stos %al,%es:(%edi)
   return (int)nk_do_syscall(SYS_FORK, a, b); 
 8048529:	8d 44 24 20          	lea    0x20(%esp),%eax
 804852d:	89 44 24 08          	mov    %eax,0x8(%esp)
 8048531:	8d 44 24 10          	lea    0x10(%esp),%eax
 8048535:	89 44 24 04          	mov    %eax,0x4(%esp)
 8048539:	c7 04 24 01 00 00 00 	movl   $0x1,(%esp)
 8048540:	e8 53 65 00 00       	call   804ea98 <nk_do_syscall>
}
 8048545:	83 c4 38             	add    $0x38,%esp
 8048548:	5f                   	pop    %edi
 8048549:	c3                   	ret    
 804854a:	66 90                	xchg   %ax,%ax

0804854c <getpid>:


int
_DEFUN (getpid, (),
        _NOARGS)
{
 804854c:	57                   	push   %edi
 804854d:	83 ec 38             	sub    $0x38,%esp
   int a[] = {0,0,0,0};
 8048550:	8d 7c 24 10          	lea    0x10(%esp),%edi
 8048554:	31 c0                	xor    %eax,%eax
 8048556:	b9 10 00 00 00       	mov    $0x10,%ecx
 804855b:	f3 aa                	rep stos %al,%es:(%edi)
   int b[] = {0,0,0,0};
 804855d:	8d 7c 24 20          	lea    0x20(%esp),%edi
 8048561:	b1 10                	mov    $0x10,%cl
 8048563:	f3 aa                	rep stos %al,%es:(%edi)
   return (int) nk_do_syscall(SYS_GETPID, a, b); 
 8048565:	8d 44 24 20          	lea    0x20(%esp),%eax
 8048569:	89 44 24 08          	mov    %eax,0x8(%esp)
 804856d:	8d 44 24 10          	lea    0x10(%esp),%eax
 8048571:	89 44 24 04          	mov    %eax,0x4(%esp)
 8048575:	c7 04 24 03 00 00 00 	movl   $0x3,(%esp)
 804857c:	e8 17 65 00 00       	call   804ea98 <nk_do_syscall>
}
 8048581:	83 c4 38             	add    $0x38,%esp
 8048584:	5f                   	pop    %edi
 8048585:	c3                   	ret    
 8048586:	66 90                	xchg   %ax,%ax

08048588 <getppid>:


int
_DEFUN (getppid, (),
        _NOARGS)
{
 8048588:	57                   	push   %edi
 8048589:	83 ec 38             	sub    $0x38,%esp
   int a[] = {0,0,0,0};
 804858c:	8d 7c 24 10          	lea    0x10(%esp),%edi
 8048590:	31 c0                	xor    %eax,%eax
 8048592:	b9 10 00 00 00       	mov    $0x10,%ecx
 8048597:	f3 aa                	rep stos %al,%es:(%edi)
   int b[] = {0,0,0,0};
 8048599:	8d 7c 24 20          	lea    0x20(%esp),%edi
 804859d:	b1 10                	mov    $0x10,%cl
 804859f:	f3 aa                	rep stos %al,%es:(%edi)
   return (int) nk_do_syscall(SYS_GETPPID, a, b); 
 80485a1:	8d 44 24 20          	lea    0x20(%esp),%eax
 80485a5:	89 44 24 08          	mov    %eax,0x8(%esp)
 80485a9:	8d 44 24 10          	lea    0x10(%esp),%eax
 80485ad:	89 44 24 04          	mov    %eax,0x4(%esp)
 80485b1:	c7 04 24 25 00 00 00 	movl   $0x25,(%esp)
 80485b8:	e8 db 64 00 00       	call   804ea98 <nk_do_syscall>
}
 80485bd:	83 c4 38             	add    $0x38,%esp
 80485c0:	5f                   	pop    %edi
 80485c1:	c3                   	ret    
 80485c2:	66 90                	xchg   %ax,%ax

080485c4 <getuid>:


uid_t
_DEFUN (getuid, (),
        _NOARGS)
{
 80485c4:	57                   	push   %edi
 80485c5:	83 ec 38             	sub    $0x38,%esp
   int a[] = {0,0,0,0};
 80485c8:	8d 7c 24 10          	lea    0x10(%esp),%edi
 80485cc:	31 c0                	xor    %eax,%eax
 80485ce:	b9 10 00 00 00       	mov    $0x10,%ecx
 80485d3:	f3 aa                	rep stos %al,%es:(%edi)
   int b[] = {0,0,0,0};
 80485d5:	8d 7c 24 20          	lea    0x20(%esp),%edi
 80485d9:	b1 10                	mov    $0x10,%cl
 80485db:	f3 aa                	rep stos %al,%es:(%edi)
   return (int) nk_do_syscall(SYS_GETUID, a, b); 
 80485dd:	8d 44 24 20          	lea    0x20(%esp),%eax
 80485e1:	89 44 24 08          	mov    %eax,0x8(%esp)
 80485e5:	8d 44 24 10          	lea    0x10(%esp),%eax
 80485e9:	89 44 24 04          	mov    %eax,0x4(%esp)
 80485ed:	c7 04 24 09 00 00 00 	movl   $0x9,(%esp)
 80485f4:	e8 9f 64 00 00       	call   804ea98 <nk_do_syscall>
}
 80485f9:	83 c4 38             	add    $0x38,%esp
 80485fc:	5f                   	pop    %edi
 80485fd:	c3                   	ret    
 80485fe:	66 90                	xchg   %ax,%ax

08048600 <getpgrp>:

pid_t
_DEFUN (getpgrp, (),
        _NOARGS)
{
 8048600:	57                   	push   %edi
 8048601:	83 ec 38             	sub    $0x38,%esp
   int a[] = {0,0,0,0};
 8048604:	8d 7c 24 10          	lea    0x10(%esp),%edi
 8048608:	31 c0                	xor    %eax,%eax
 804860a:	b9 10 00 00 00       	mov    $0x10,%ecx
 804860f:	f3 aa                	rep stos %al,%es:(%edi)
   int b[] = {0,0,0,0};
 8048611:	8d 7c 24 20          	lea    0x20(%esp),%edi
 8048615:	b1 10                	mov    $0x10,%cl
 8048617:	f3 aa                	rep stos %al,%es:(%edi)
   return (int) nk_do_syscall(SYS_GETPGRP, a, b); 
 8048619:	8d 44 24 20          	lea    0x20(%esp),%eax
 804861d:	89 44 24 08          	mov    %eax,0x8(%esp)
 8048621:	8d 44 24 10          	lea    0x10(%esp),%eax
 8048625:	89 44 24 04          	mov    %eax,0x4(%esp)
 8048629:	c7 04 24 04 00 00 00 	movl   $0x4,(%esp)
 8048630:	e8 63 64 00 00       	call   804ea98 <nk_do_syscall>
}
 8048635:	83 c4 38             	add    $0x38,%esp
 8048638:	5f                   	pop    %edi
 8048639:	c3                   	ret    
 804863a:	66 90                	xchg   %ax,%ax

0804863c <getsid>:

pid_t
_DEFUN (getsid, (),
        _NOARGS)
{
 804863c:	57                   	push   %edi
 804863d:	83 ec 38             	sub    $0x38,%esp
   int a[] = {0,0,0,0};
 8048640:	8d 7c 24 10          	lea    0x10(%esp),%edi
 8048644:	31 c0                	xor    %eax,%eax
 8048646:	b9 10 00 00 00       	mov    $0x10,%ecx
 804864b:	f3 aa                	rep stos %al,%es:(%edi)
   int b[] = {0,0,0,0};
 804864d:	8d 7c 24 20          	lea    0x20(%esp),%edi
 8048651:	b1 10                	mov    $0x10,%cl
 8048653:	f3 aa                	rep stos %al,%es:(%edi)
   return (int) nk_do_syscall(SYS_GETSID, a, b); 
 8048655:	8d 44 24 20          	lea    0x20(%esp),%eax
 8048659:	89 44 24 08          	mov    %eax,0x8(%esp)
 804865d:	8d 44 24 10          	lea    0x10(%esp),%eax
 8048661:	89 44 24 04          	mov    %eax,0x4(%esp)
 8048665:	c7 04 24 2e 00 00 00 	movl   $0x2e,(%esp)
 804866c:	e8 27 64 00 00       	call   804ea98 <nk_do_syscall>
}
 8048671:	83 c4 38             	add    $0x38,%esp
 8048674:	5f                   	pop    %edi
 8048675:	c3                   	ret    
 8048676:	66 90                	xchg   %ax,%ax

08048678 <geteuid>:

uid_t
_DEFUN (geteuid, (),
        _NOARGS)
{
 8048678:	57                   	push   %edi
 8048679:	83 ec 38             	sub    $0x38,%esp
   int a[] = {0,0,0,0};
 804867c:	8d 7c 24 10          	lea    0x10(%esp),%edi
 8048680:	31 c0                	xor    %eax,%eax
 8048682:	b9 10 00 00 00       	mov    $0x10,%ecx
 8048687:	f3 aa                	rep stos %al,%es:(%edi)
   int b[] = {0,0,0,0};
 8048689:	8d 7c 24 20          	lea    0x20(%esp),%edi
 804868d:	b1 10                	mov    $0x10,%cl
 804868f:	f3 aa                	rep stos %al,%es:(%edi)
   return (int) nk_do_syscall(SYS_GETEUID, a, b); 
 8048691:	8d 44 24 20          	lea    0x20(%esp),%eax
 8048695:	89 44 24 08          	mov    %eax,0x8(%esp)
 8048699:	8d 44 24 10          	lea    0x10(%esp),%eax
 804869d:	89 44 24 04          	mov    %eax,0x4(%esp)
 80486a1:	c7 04 24 0a 00 00 00 	movl   $0xa,(%esp)
 80486a8:	e8 eb 63 00 00       	call   804ea98 <nk_do_syscall>
}
 80486ad:	83 c4 38             	add    $0x38,%esp
 80486b0:	5f                   	pop    %edi
 80486b1:	c3                   	ret    
 80486b2:	66 90                	xchg   %ax,%ax

080486b4 <getgid>:

gid_t
_DEFUN (getgid, (),
        _NOARGS)
{
 80486b4:	57                   	push   %edi
 80486b5:	83 ec 38             	sub    $0x38,%esp
   int a[] = {0,0,0,0};
 80486b8:	8d 7c 24 10          	lea    0x10(%esp),%edi
 80486bc:	31 c0                	xor    %eax,%eax
 80486be:	b9 10 00 00 00       	mov    $0x10,%ecx
 80486c3:	f3 aa                	rep stos %al,%es:(%edi)
   int b[] = {0,0,0,0};
 80486c5:	8d 7c 24 20          	lea    0x20(%esp),%edi
 80486c9:	b1 10                	mov    $0x10,%cl
 80486cb:	f3 aa                	rep stos %al,%es:(%edi)
   return (int) nk_do_syscall(SYS_GETGID, a, b); 
 80486cd:	8d 44 24 20          	lea    0x20(%esp),%eax
 80486d1:	89 44 24 08          	mov    %eax,0x8(%esp)
 80486d5:	8d 44 24 10          	lea    0x10(%esp),%eax
 80486d9:	89 44 24 04          	mov    %eax,0x4(%esp)
 80486dd:	c7 04 24 0b 00 00 00 	movl   $0xb,(%esp)
 80486e4:	e8 af 63 00 00       	call   804ea98 <nk_do_syscall>
}
 80486e9:	83 c4 38             	add    $0x38,%esp
 80486ec:	5f                   	pop    %edi
 80486ed:	c3                   	ret    
 80486ee:	66 90                	xchg   %ax,%ax

080486f0 <getegid>:

gid_t
_DEFUN (getegid, (),
        _NOARGS)
{
 80486f0:	57                   	push   %edi
 80486f1:	83 ec 38             	sub    $0x38,%esp
   int a[] = {0,0,0,0};
 80486f4:	8d 7c 24 10          	lea    0x10(%esp),%edi
 80486f8:	31 c0                	xor    %eax,%eax
 80486fa:	b9 10 00 00 00       	mov    $0x10,%ecx
 80486ff:	f3 aa                	rep stos %al,%es:(%edi)
   int b[] = {0,0,0,0};
 8048701:	8d 7c 24 20          	lea    0x20(%esp),%edi
 8048705:	b1 10                	mov    $0x10,%cl
 8048707:	f3 aa                	rep stos %al,%es:(%edi)
   return (int) nk_do_syscall(SYS_GETEGID, a, b); 
 8048709:	8d 44 24 20          	lea    0x20(%esp),%eax
 804870d:	89 44 24 08          	mov    %eax,0x8(%esp)
 8048711:	8d 44 24 10          	lea    0x10(%esp),%eax
 8048715:	89 44 24 04          	mov    %eax,0x4(%esp)
 8048719:	c7 04 24 0c 00 00 00 	movl   $0xc,(%esp)
 8048720:	e8 73 63 00 00       	call   804ea98 <nk_do_syscall>
}
 8048725:	83 c4 38             	add    $0x38,%esp
 8048728:	5f                   	pop    %edi
 8048729:	c3                   	ret    
 804872a:	66 90                	xchg   %ax,%ax

0804872c <setsid>:

pid_t
_DEFUN (setsid, (),
        _NOARGS)
{
 804872c:	57                   	push   %edi
 804872d:	83 ec 38             	sub    $0x38,%esp
   int a[] = {0,0,0,0};
 8048730:	8d 7c 24 10          	lea    0x10(%esp),%edi
 8048734:	31 c0                	xor    %eax,%eax
 8048736:	b9 10 00 00 00       	mov    $0x10,%ecx
 804873b:	f3 aa                	rep stos %al,%es:(%edi)
   int b[] = {0,0,0,0};
 804873d:	8d 7c 24 20          	lea    0x20(%esp),%edi
 8048741:	b1 10                	mov    $0x10,%cl
 8048743:	f3 aa                	rep stos %al,%es:(%edi)
   return (int) nk_do_syscall(SYS_SETSID, a, b); 
 8048745:	8d 44 24 20          	lea    0x20(%esp),%eax
 8048749:	89 44 24 08          	mov    %eax,0x8(%esp)
 804874d:	8d 44 24 10          	lea    0x10(%esp),%eax
 8048751:	89 44 24 04          	mov    %eax,0x4(%esp)
 8048755:	c7 04 24 05 00 00 00 	movl   $0x5,(%esp)
 804875c:	e8 37 63 00 00       	call   804ea98 <nk_do_syscall>
}
 8048761:	83 c4 38             	add    $0x38,%esp
 8048764:	5f                   	pop    %edi
 8048765:	c3                   	ret    
 8048766:	66 90                	xchg   %ax,%ax

08048768 <setpgrp>:

int
_DEFUN (setpgrp, (),
        _NOARGS)
{
 8048768:	57                   	push   %edi
 8048769:	83 ec 38             	sub    $0x38,%esp
   int a[] = {0,0,0,0};
 804876c:	8d 7c 24 10          	lea    0x10(%esp),%edi
 8048770:	31 c0                	xor    %eax,%eax
 8048772:	b9 10 00 00 00       	mov    $0x10,%ecx
 8048777:	f3 aa                	rep stos %al,%es:(%edi)
   int b[] = {0,0,0,0};
 8048779:	8d 7c 24 20          	lea    0x20(%esp),%edi
 804877d:	b1 10                	mov    $0x10,%cl
 804877f:	f3 aa                	rep stos %al,%es:(%edi)
   return (int) nk_do_syscall(SYS_SETPGRP, a, b); 
 8048781:	8d 44 24 20          	lea    0x20(%esp),%eax
 8048785:	89 44 24 08          	mov    %eax,0x8(%esp)
 8048789:	8d 44 24 10          	lea    0x10(%esp),%eax
 804878d:	89 44 24 04          	mov    %eax,0x4(%esp)
 8048791:	c7 04 24 2d 00 00 00 	movl   $0x2d,(%esp)
 8048798:	e8 fb 62 00 00       	call   804ea98 <nk_do_syscall>
}
 804879d:	83 c4 38             	add    $0x38,%esp
 80487a0:	5f                   	pop    %edi
 80487a1:	c3                   	ret    
 80487a2:	66 90                	xchg   %ax,%ax

080487a4 <setuid>:

int setuid(int uid) {
 80487a4:	57                   	push   %edi
 80487a5:	83 ec 38             	sub    $0x38,%esp
   int a[] = {uid,0,0,0};
 80487a8:	8b 44 24 40          	mov    0x40(%esp),%eax
 80487ac:	89 44 24 10          	mov    %eax,0x10(%esp)
 80487b0:	c7 44 24 14 00 00 00 	movl   $0x0,0x14(%esp)
 80487b7:	00 
 80487b8:	c7 44 24 18 00 00 00 	movl   $0x0,0x18(%esp)
 80487bf:	00 
 80487c0:	c7 44 24 1c 00 00 00 	movl   $0x0,0x1c(%esp)
 80487c7:	00 
   int b[] = {0,0,0,0};
 80487c8:	8d 7c 24 20          	lea    0x20(%esp),%edi
 80487cc:	b9 10 00 00 00       	mov    $0x10,%ecx
 80487d1:	31 c0                	xor    %eax,%eax
 80487d3:	f3 aa                	rep stos %al,%es:(%edi)
   return (int) nk_do_syscall(SYS_SETUID, a, b); 
 80487d5:	8d 44 24 20          	lea    0x20(%esp),%eax
 80487d9:	89 44 24 08          	mov    %eax,0x8(%esp)
 80487dd:	8d 44 24 10          	lea    0x10(%esp),%eax
 80487e1:	89 44 24 04          	mov    %eax,0x4(%esp)
 80487e5:	c7 04 24 34 00 00 00 	movl   $0x34,(%esp)
 80487ec:	e8 a7 62 00 00       	call   804ea98 <nk_do_syscall>
}
 80487f1:	83 c4 38             	add    $0x38,%esp
 80487f4:	5f                   	pop    %edi
 80487f5:	c3                   	ret    
 80487f6:	66 90                	xchg   %ax,%ax

080487f8 <setgid>:

int setgid(int gid) {
 80487f8:	57                   	push   %edi
 80487f9:	83 ec 38             	sub    $0x38,%esp
   int a[] = {gid,0,0,0};
 80487fc:	8b 44 24 40          	mov    0x40(%esp),%eax
 8048800:	89 44 24 10          	mov    %eax,0x10(%esp)
 8048804:	c7 44 24 14 00 00 00 	movl   $0x0,0x14(%esp)
 804880b:	00 
 804880c:	c7 44 24 18 00 00 00 	movl   $0x0,0x18(%esp)
 8048813:	00 
 8048814:	c7 44 24 1c 00 00 00 	movl   $0x0,0x1c(%esp)
 804881b:	00 
   int b[] = {0,0,0,0};
 804881c:	8d 7c 24 20          	lea    0x20(%esp),%edi
 8048820:	b9 10 00 00 00       	mov    $0x10,%ecx
 8048825:	31 c0                	xor    %eax,%eax
 8048827:	f3 aa                	rep stos %al,%es:(%edi)
   return (int) nk_do_syscall(SYS_SETGID, a, b); 
 8048829:	8d 44 24 20          	lea    0x20(%esp),%eax
 804882d:	89 44 24 08          	mov    %eax,0x8(%esp)
 8048831:	8d 44 24 10          	lea    0x10(%esp),%eax
 8048835:	89 44 24 04          	mov    %eax,0x4(%esp)
 8048839:	c7 04 24 35 00 00 00 	movl   $0x35,(%esp)
 8048840:	e8 53 62 00 00       	call   804ea98 <nk_do_syscall>
}
 8048845:	83 c4 38             	add    $0x38,%esp
 8048848:	5f                   	pop    %edi
 8048849:	c3                   	ret    
 804884a:	66 90                	xchg   %ax,%ax

0804884c <setresuid>:

int setresuid(int uid) {
	return 0;
}
 804884c:	31 c0                	xor    %eax,%eax
 804884e:	c3                   	ret    
 804884f:	90                   	nop

08048850 <setresgid>:

int setresgid(int gid) {
	return 0;
}
 8048850:	31 c0                	xor    %eax,%eax
 8048852:	c3                   	ret    
 8048853:	90                   	nop

08048854 <getgrgid>:

int getgrgid(){
 8048854:	83 ec 0c             	sub    $0xc,%esp
	return getgid();
 8048857:	e8 58 fe ff ff       	call   80486b4 <getgid>
 804885c:	0f b7 c0             	movzwl %ax,%eax
}
 804885f:	83 c4 0c             	add    $0xc,%esp
 8048862:	c3                   	ret    
 8048863:	90                   	nop

08048864 <setpgid>:

int setpgid(pid_t pid, pid_t pgid) {
 8048864:	57                   	push   %edi
 8048865:	83 ec 38             	sub    $0x38,%esp
   int a[] = {(int)pid,(int)pgid,0,0};
 8048868:	8b 44 24 40          	mov    0x40(%esp),%eax
 804886c:	89 44 24 10          	mov    %eax,0x10(%esp)
 8048870:	8b 44 24 44          	mov    0x44(%esp),%eax
 8048874:	89 44 24 14          	mov    %eax,0x14(%esp)
 8048878:	c7 44 24 18 00 00 00 	movl   $0x0,0x18(%esp)
 804887f:	00 
 8048880:	c7 44 24 1c 00 00 00 	movl   $0x0,0x1c(%esp)
 8048887:	00 
   int b[] = {0,0,0,0};
 8048888:	8d 7c 24 20          	lea    0x20(%esp),%edi
 804888c:	b9 10 00 00 00       	mov    $0x10,%ecx
 8048891:	31 c0                	xor    %eax,%eax
 8048893:	f3 aa                	rep stos %al,%es:(%edi)
   return (int) nk_do_syscall(SYS_SETPGID, a, b); 
 8048895:	8d 44 24 20          	lea    0x20(%esp),%eax
 8048899:	89 44 24 08          	mov    %eax,0x8(%esp)
 804889d:	8d 44 24 10          	lea    0x10(%esp),%eax
 80488a1:	89 44 24 04          	mov    %eax,0x4(%esp)
 80488a5:	c7 04 24 2f 00 00 00 	movl   $0x2f,(%esp)
 80488ac:	e8 e7 61 00 00       	call   804ea98 <nk_do_syscall>
}
 80488b1:	83 c4 38             	add    $0x38,%esp
 80488b4:	5f                   	pop    %edi
 80488b5:	c3                   	ret    
 80488b6:	66 90                	xchg   %ax,%ax

080488b8 <signal>:

typedef void (*sighandler_t)(int);

sighandler_t signal(int signum, sighandler_t handler)
{
 80488b8:	57                   	push   %edi
 80488b9:	83 ec 38             	sub    $0x38,%esp
   int a[] = {(int)signum,(int)handler,0,0};
 80488bc:	8b 44 24 40          	mov    0x40(%esp),%eax
 80488c0:	89 44 24 10          	mov    %eax,0x10(%esp)
 80488c4:	8b 44 24 44          	mov    0x44(%esp),%eax
 80488c8:	89 44 24 14          	mov    %eax,0x14(%esp)
 80488cc:	c7 44 24 18 00 00 00 	movl   $0x0,0x18(%esp)
 80488d3:	00 
 80488d4:	c7 44 24 1c 00 00 00 	movl   $0x0,0x1c(%esp)
 80488db:	00 
   int b[] = {0,0,0,0};
 80488dc:	8d 7c 24 20          	lea    0x20(%esp),%edi
 80488e0:	b9 10 00 00 00       	mov    $0x10,%ecx
 80488e5:	31 c0                	xor    %eax,%eax
 80488e7:	f3 aa                	rep stos %al,%es:(%edi)
   return (int) nk_do_syscall(SYS_SIGNAL, a, b); 
 80488e9:	8d 44 24 20          	lea    0x20(%esp),%eax
 80488ed:	89 44 24 08          	mov    %eax,0x8(%esp)
 80488f1:	8d 44 24 10          	lea    0x10(%esp),%eax
 80488f5:	89 44 24 04          	mov    %eax,0x4(%esp)
 80488f9:	c7 04 24 36 00 00 00 	movl   $0x36,(%esp)
 8048900:	e8 93 61 00 00       	call   804ea98 <nk_do_syscall>
}
 8048905:	83 c4 38             	add    $0x38,%esp
 8048908:	5f                   	pop    %edi
 8048909:	c3                   	ret    
 804890a:	66 90                	xchg   %ax,%ax

0804890c <exitsig>:

void exitsig()
{
 804890c:	57                   	push   %edi
 804890d:	83 ec 38             	sub    $0x38,%esp
   int a[] = {0,0,0,0};
 8048910:	8d 7c 24 10          	lea    0x10(%esp),%edi
 8048914:	31 c0                	xor    %eax,%eax
 8048916:	b9 10 00 00 00       	mov    $0x10,%ecx
 804891b:	f3 aa                	rep stos %al,%es:(%edi)
   int b[] = {0,0,0,0};
 804891d:	8d 7c 24 20          	lea    0x20(%esp),%edi
 8048921:	b1 10                	mov    $0x10,%cl
 8048923:	f3 aa                	rep stos %al,%es:(%edi)
   return (int) nk_do_syscall(SYS_EXITSIG, a, b); 
 8048925:	8d 44 24 20          	lea    0x20(%esp),%eax
 8048929:	89 44 24 08          	mov    %eax,0x8(%esp)
 804892d:	8d 44 24 10          	lea    0x10(%esp),%eax
 8048931:	89 44 24 04          	mov    %eax,0x4(%esp)
 8048935:	c7 04 24 37 00 00 00 	movl   $0x37,(%esp)
 804893c:	e8 57 61 00 00       	call   804ea98 <nk_do_syscall>
	
}
 8048941:	83 c4 38             	add    $0x38,%esp
 8048944:	5f                   	pop    %edi
 8048945:	c3                   	ret    
 8048946:	66 90                	xchg   %ax,%ax

08048948 <getgroups>:
}

int getgroups(int size, gid_t list[])
{
	return 0;//TODO: Implement getgroups
}
 8048948:	31 c0                	xor    %eax,%eax
 804894a:	c3                   	ret    
 804894b:	90                   	nop

0804894c <sigaction>:

int sigaction(int signum, const struct sigaction *act,
                     struct sigaction *oldact)
{
	return 0;
}
 804894c:	31 c0                	xor    %eax,%eax
 804894e:	c3                   	ret    
 804894f:	90                   	nop

08048950 <sigprocmask>:

int sigprocmask(int how, const sigset_t *set, sigset_t *oldset)
{
	return 0;
}
 8048950:	31 c0                	xor    %eax,%eax
 8048952:	c3                   	ret    
 8048953:	90                   	nop

08048954 <alarm>:

unsigned int alarm(unsigned int seconds)
{
	return 0;
}
 8048954:	31 c0                	xor    %eax,%eax
 8048956:	c3                   	ret    
 8048957:	90                   	nop

08048958 <sleep>:

unsigned int sleep(unsigned int seconds)
{
 8048958:	57                   	push   %edi
 8048959:	83 ec 38             	sub    $0x38,%esp
   int a[] = {(uint32_t) seconds, 0,0,0};
 804895c:	8b 44 24 40          	mov    0x40(%esp),%eax
 8048960:	89 44 24 10          	mov    %eax,0x10(%esp)
 8048964:	c7 44 24 14 00 00 00 	movl   $0x0,0x14(%esp)
 804896b:	00 
 804896c:	c7 44 24 18 00 00 00 	movl   $0x0,0x18(%esp)
 8048973:	00 
 8048974:	c7 44 24 1c 00 00 00 	movl   $0x0,0x1c(%esp)
 804897b:	00 
   int b[] = {0,0,0,0};
 804897c:	8d 7c 24 20          	lea    0x20(%esp),%edi
 8048980:	b9 10 00 00 00       	mov    $0x10,%ecx
 8048985:	31 c0                	xor    %eax,%eax
 8048987:	f3 aa                	rep stos %al,%es:(%edi)
   return nk_do_syscall(SYS_SLEEP, a, b); 
 8048989:	8d 44 24 20          	lea    0x20(%esp),%eax
 804898d:	89 44 24 08          	mov    %eax,0x8(%esp)
 8048991:	8d 44 24 10          	lea    0x10(%esp),%eax
 8048995:	89 44 24 04          	mov    %eax,0x4(%esp)
 8048999:	c7 04 24 32 00 00 00 	movl   $0x32,(%esp)
 80489a0:	e8 f3 60 00 00       	call   804ea98 <nk_do_syscall>
}
 80489a5:	83 c4 38             	add    $0x38,%esp
 80489a8:	5f                   	pop    %edi
 80489a9:	c3                   	ret    
 80489aa:	66 90                	xchg   %ax,%ax

080489ac <nanosleep>:

int nanosleep(const struct timespec *req, struct timespec *rem)
{
 80489ac:	83 ec 1c             	sub    $0x1c,%esp
	sleep((unsigned int)req->tv_sec);
 80489af:	8b 44 24 20          	mov    0x20(%esp),%eax
 80489b3:	8b 00                	mov    (%eax),%eax
 80489b5:	89 04 24             	mov    %eax,(%esp)
 80489b8:	e8 9b ff ff ff       	call   8048958 <sleep>
	return 0;
}
 80489bd:	31 c0                	xor    %eax,%eax
 80489bf:	83 c4 1c             	add    $0x1c,%esp
 80489c2:	c3                   	ret    
 80489c3:	90                   	nop

080489c4 <stime>:

int stime(time_t *t)
{
 80489c4:	57                   	push   %edi
 80489c5:	83 ec 38             	sub    $0x38,%esp
   int a[] = {(uint32_t) t, 0,0,0};
 80489c8:	8b 44 24 40          	mov    0x40(%esp),%eax
 80489cc:	89 44 24 10          	mov    %eax,0x10(%esp)
 80489d0:	c7 44 24 14 00 00 00 	movl   $0x0,0x14(%esp)
 80489d7:	00 
 80489d8:	c7 44 24 18 00 00 00 	movl   $0x0,0x18(%esp)
 80489df:	00 
 80489e0:	c7 44 24 1c 00 00 00 	movl   $0x0,0x1c(%esp)
 80489e7:	00 
   int b[] = {0,0,0,0};
 80489e8:	8d 7c 24 20          	lea    0x20(%esp),%edi
 80489ec:	b9 10 00 00 00       	mov    $0x10,%ecx
 80489f1:	31 c0                	xor    %eax,%eax
 80489f3:	f3 aa                	rep stos %al,%es:(%edi)
   return nk_do_syscall(SYS_STIME, a, b); 
 80489f5:	8d 44 24 20          	lea    0x20(%esp),%eax
 80489f9:	89 44 24 08          	mov    %eax,0x8(%esp)
 80489fd:	8d 44 24 10          	lea    0x10(%esp),%eax
 8048a01:	89 44 24 04          	mov    %eax,0x4(%esp)
 8048a05:	c7 04 24 33 00 00 00 	movl   $0x33,(%esp)
 8048a0c:	e8 87 60 00 00       	call   804ea98 <nk_do_syscall>
}
 8048a11:	83 c4 38             	add    $0x38,%esp
 8048a14:	5f                   	pop    %edi
 8048a15:	c3                   	ret    
 8048a16:	66 90                	xchg   %ax,%ax

08048a18 <usleep>:

int usleep(useconds_t usec)
{
 8048a18:	57                   	push   %edi
 8048a19:	83 ec 38             	sub    $0x38,%esp
   int a[] = {(uint32_t) usec, 0,0,0};
 8048a1c:	8b 44 24 40          	mov    0x40(%esp),%eax
 8048a20:	89 44 24 10          	mov    %eax,0x10(%esp)
 8048a24:	c7 44 24 14 00 00 00 	movl   $0x0,0x14(%esp)
 8048a2b:	00 
 8048a2c:	c7 44 24 18 00 00 00 	movl   $0x0,0x18(%esp)
 8048a33:	00 
 8048a34:	c7 44 24 1c 00 00 00 	movl   $0x0,0x1c(%esp)
 8048a3b:	00 
   int b[] = {0,0,0,0};
 8048a3c:	8d 7c 24 20          	lea    0x20(%esp),%edi
 8048a40:	b9 10 00 00 00       	mov    $0x10,%ecx
 8048a45:	31 c0                	xor    %eax,%eax
 8048a47:	f3 aa                	rep stos %al,%es:(%edi)
   return nk_do_syscall(SYS_USLEEP, a, b); 
 8048a49:	8d 44 24 20          	lea    0x20(%esp),%eax
 8048a4d:	89 44 24 08          	mov    %eax,0x8(%esp)
 8048a51:	8d 44 24 10          	lea    0x10(%esp),%eax
 8048a55:	89 44 24 04          	mov    %eax,0x4(%esp)
 8048a59:	c7 04 24 31 00 00 00 	movl   $0x31,(%esp)
 8048a60:	e8 33 60 00 00       	call   804ea98 <nk_do_syscall>
}
 8048a65:	83 c4 38             	add    $0x38,%esp
 8048a68:	5f                   	pop    %edi
 8048a69:	c3                   	ret    
 8048a6a:	66 90                	xchg   %ax,%ax

08048a6c <kill>:

int
_DEFUN (kill, (pid, sig),
        int pid  _AND
        int sig)
{  
 8048a6c:	57                   	push   %edi
 8048a6d:	83 ec 38             	sub    $0x38,%esp
   int a[] = {pid,sig,0,0};
 8048a70:	8b 44 24 40          	mov    0x40(%esp),%eax
 8048a74:	89 44 24 10          	mov    %eax,0x10(%esp)
 8048a78:	8b 44 24 44          	mov    0x44(%esp),%eax
 8048a7c:	89 44 24 14          	mov    %eax,0x14(%esp)
 8048a80:	c7 44 24 18 00 00 00 	movl   $0x0,0x18(%esp)
 8048a87:	00 
 8048a88:	c7 44 24 1c 00 00 00 	movl   $0x0,0x1c(%esp)
 8048a8f:	00 
   int b[] = {0,0,0,0};
 8048a90:	8d 7c 24 20          	lea    0x20(%esp),%edi
 8048a94:	b9 10 00 00 00       	mov    $0x10,%ecx
 8048a99:	31 c0                	xor    %eax,%eax
 8048a9b:	f3 aa                	rep stos %al,%es:(%edi)
   return (int) nk_do_syscall(SYS_KILL, a, b); 
 8048a9d:	8d 44 24 20          	lea    0x20(%esp),%eax
 8048aa1:	89 44 24 08          	mov    %eax,0x8(%esp)
 8048aa5:	8d 44 24 10          	lea    0x10(%esp),%eax
 8048aa9:	89 44 24 04          	mov    %eax,0x4(%esp)
 8048aad:	c7 04 24 02 00 00 00 	movl   $0x2,(%esp)
 8048ab4:	e8 df 5f 00 00       	call   804ea98 <nk_do_syscall>
}
 8048ab9:	83 c4 38             	add    $0x38,%esp
 8048abc:	5f                   	pop    %edi
 8048abd:	c3                   	ret    
 8048abe:	66 90                	xchg   %ax,%ax

08048ac0 <killpg>:
   return (int) nk_do_syscall(SYS_EXITSIG, a, b); 
	
}

int killpg(int pgrp, int sig)
{
 8048ac0:	8b 44 24 04          	mov    0x4(%esp),%eax
	return kill(-pgrp, sig);
 8048ac4:	f7 d8                	neg    %eax
 8048ac6:	89 44 24 04          	mov    %eax,0x4(%esp)
 8048aca:	e9 9d ff ff ff       	jmp    8048a6c <kill>
 8048acf:	90                   	nop

08048ad0 <sbrk>:
}

void *
sbrk (incr)
     int incr;
{ 
 8048ad0:	57                   	push   %edi
 8048ad1:	83 ec 38             	sub    $0x38,%esp
   int a[] = {(uint32_t) incr,0,0,0};
 8048ad4:	8b 44 24 40          	mov    0x40(%esp),%eax
 8048ad8:	89 44 24 10          	mov    %eax,0x10(%esp)
 8048adc:	c7 44 24 14 00 00 00 	movl   $0x0,0x14(%esp)
 8048ae3:	00 
 8048ae4:	c7 44 24 18 00 00 00 	movl   $0x0,0x18(%esp)
 8048aeb:	00 
 8048aec:	c7 44 24 1c 00 00 00 	movl   $0x0,0x1c(%esp)
 8048af3:	00 
   int b[] = {0,0,0,0};
 8048af4:	8d 7c 24 20          	lea    0x20(%esp),%edi
 8048af8:	b9 10 00 00 00       	mov    $0x10,%ecx
 8048afd:	31 c0                	xor    %eax,%eax
 8048aff:	f3 aa                	rep stos %al,%es:(%edi)
   return (void *) nk_do_syscall(SYS_SBRK, a, b); 
 8048b01:	8d 44 24 20          	lea    0x20(%esp),%eax
 8048b05:	89 44 24 08          	mov    %eax,0x8(%esp)
 8048b09:	8d 44 24 10          	lea    0x10(%esp),%eax
 8048b0d:	89 44 24 04          	mov    %eax,0x4(%esp)
 8048b11:	c7 04 24 08 00 00 00 	movl   $0x8,(%esp)
 8048b18:	e8 7b 5f 00 00       	call   804ea98 <nk_do_syscall>
} 
 8048b1d:	83 c4 38             	add    $0x38,%esp
 8048b20:	5f                   	pop    %edi
 8048b21:	c3                   	ret    
 8048b22:	66 90                	xchg   %ax,%ax

08048b24 <waitpid>:
int
_DEFUN (waitpid, (pid, status, options),
	pid_t pid _AND
        int  *status _AND
	int  options)
{
 8048b24:	57                   	push   %edi
 8048b25:	83 ec 38             	sub    $0x38,%esp
	//Calls WAITPID!!!
   uint32_t a[] = {(uint32_t) pid, (uint32_t) status, (uint32_t) options, 0};
 8048b28:	8b 44 24 40          	mov    0x40(%esp),%eax
 8048b2c:	89 44 24 10          	mov    %eax,0x10(%esp)
 8048b30:	8b 44 24 44          	mov    0x44(%esp),%eax
 8048b34:	89 44 24 14          	mov    %eax,0x14(%esp)
 8048b38:	8b 44 24 48          	mov    0x48(%esp),%eax
 8048b3c:	89 44 24 18          	mov    %eax,0x18(%esp)
 8048b40:	c7 44 24 1c 00 00 00 	movl   $0x0,0x1c(%esp)
 8048b47:	00 
   uint32_t b[] = {0, 0, 0, 0};
 8048b48:	8d 7c 24 20          	lea    0x20(%esp),%edi
 8048b4c:	b9 10 00 00 00       	mov    $0x10,%ecx
 8048b51:	31 c0                	xor    %eax,%eax
 8048b53:	f3 aa                	rep stos %al,%es:(%edi)
   return (int) nk_do_syscall(SYS_WAITPID, a, b); 
 8048b55:	8d 44 24 20          	lea    0x20(%esp),%eax
 8048b59:	89 44 24 08          	mov    %eax,0x8(%esp)
 8048b5d:	8d 44 24 10          	lea    0x10(%esp),%eax
 8048b61:	89 44 24 04          	mov    %eax,0x4(%esp)
 8048b65:	c7 04 24 24 00 00 00 	movl   $0x24,(%esp)
 8048b6c:	e8 27 5f 00 00       	call   804ea98 <nk_do_syscall>
}
 8048b71:	83 c4 38             	add    $0x38,%esp
 8048b74:	5f                   	pop    %edi
 8048b75:	c3                   	ret    
 8048b76:	66 90                	xchg   %ax,%ax

08048b78 <wait3>:
}

pid_t wait3(int *status, int options,
                   struct rusage *rusage)
{
	return waitpid(-1,status,options);
 8048b78:	8b 44 24 08          	mov    0x8(%esp),%eax
 8048b7c:	89 44 24 0c          	mov    %eax,0xc(%esp)
 8048b80:	8b 44 24 04          	mov    0x4(%esp),%eax
 8048b84:	89 44 24 08          	mov    %eax,0x8(%esp)
 8048b88:	c7 44 24 04 ff ff ff 	movl   $0xffffffff,0x4(%esp)
 8048b8f:	ff 
 8048b90:	e9 8f ff ff ff       	jmp    8048b24 <waitpid>
 8048b95:	8d 76 00             	lea    0x0(%esi),%esi

08048b98 <wait>:
{
	return 0;//TODO: Implement getgroups
}
 
pid_t wait(int *status)
{
 8048b98:	83 ec 1c             	sub    $0x1c,%esp
	return waitpid (-1,status,0);
 8048b9b:	c7 44 24 08 00 00 00 	movl   $0x0,0x8(%esp)
 8048ba2:	00 
 8048ba3:	8b 44 24 20          	mov    0x20(%esp),%eax
 8048ba7:	89 44 24 04          	mov    %eax,0x4(%esp)
 8048bab:	c7 04 24 ff ff ff ff 	movl   $0xffffffff,(%esp)
 8048bb2:	e8 6d ff ff ff       	call   8048b24 <waitpid>
}
 8048bb7:	83 c4 1c             	add    $0x1c,%esp
 8048bba:	c3                   	ret    
 8048bbb:	90                   	nop

08048bbc <times>:
}

clock_t
_DEFUN (times, (buf),
        struct tms *buf)
{
 8048bbc:	83 ec 0c             	sub    $0xc,%esp
  errno = ENOSYS;
 8048bbf:	e8 3c 39 00 00       	call   804c500 <__errno>
 8048bc4:	c7 00 58 00 00 00    	movl   $0x58,(%eax)
  return -1;
}
 8048bca:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
 8048bcf:	83 c4 0c             	add    $0xc,%esp
 8048bd2:	c3                   	ret    
 8048bd3:	90                   	nop

08048bd4 <clearenv>:

int clearenv(){
	return 0;
}
 8048bd4:	31 c0                	xor    %eax,%eax
 8048bd6:	c3                   	ret    
 8048bd7:	90                   	nop

08048bd8 <getgrouplist>:

int getgrouplist(const char *user, gid_t group,
                        gid_t *groups, int *ngroups)
{
	*ngroups = 0;
 8048bd8:	8b 44 24 10          	mov    0x10(%esp),%eax
 8048bdc:	c7 00 00 00 00 00    	movl   $0x0,(%eax)
	return *ngroups;
}
 8048be2:	31 c0                	xor    %eax,%eax
 8048be4:	c3                   	ret    
 8048be5:	90                   	nop
 8048be6:	90                   	nop
 8048be7:	90                   	nop
 8048be8:	90                   	nop
 8048be9:	90                   	nop
 8048bea:	90                   	nop
 8048beb:	90                   	nop
 8048bec:	90                   	nop
 8048bed:	90                   	nop
 8048bee:	90                   	nop
 8048bef:	90                   	nop

08048bf0 <strlen>:
	.global SYM (strlen)
       SOTYPE_FUNCTION(strlen)

SYM (strlen):

	pushl ebp
 8048bf0:	55                   	push   %ebp
	movl esp,ebp
 8048bf1:	89 e5                	mov    %esp,%ebp
	pushl edi
 8048bf3:	57                   	push   %edi
	movl 8(ebp),edx
 8048bf4:	8b 55 08             	mov    0x8(%ebp),%edx
#else
/* Modern x86 hardware is much faster at double-word
   manipulation than with bytewise repnz scasb.  */

/* Do byte-wise checks until string is aligned.  */
	movl edx,edi
 8048bf7:	89 d7                	mov    %edx,%edi
	test $3,edi
 8048bf9:	f7 c7 03 00 00 00    	test   $0x3,%edi
	je L5
 8048bff:	74 25                	je     8048c26 <L5>
	movb (edi),cl
 8048c01:	8a 0f                	mov    (%edi),%cl
	incl edi
 8048c03:	47                   	inc    %edi
	testb cl,cl
 8048c04:	84 c9                	test   %cl,%cl
	je L15
 8048c06:	74 56                	je     8048c5e <L15>

	test $3,edi
 8048c08:	f7 c7 03 00 00 00    	test   $0x3,%edi
	je L5
 8048c0e:	74 16                	je     8048c26 <L5>
	movb (edi),cl
 8048c10:	8a 0f                	mov    (%edi),%cl
	incl edi
 8048c12:	47                   	inc    %edi
	testb cl,cl
 8048c13:	84 c9                	test   %cl,%cl
	je L15
 8048c15:	74 47                	je     8048c5e <L15>

	test $3,edi
 8048c17:	f7 c7 03 00 00 00    	test   $0x3,%edi
	je L5
 8048c1d:	74 07                	je     8048c26 <L5>
	movb (edi),cl
 8048c1f:	8a 0f                	mov    (%edi),%cl
	incl edi
 8048c21:	47                   	inc    %edi
	testb cl,cl
 8048c22:	84 c9                	test   %cl,%cl
	je L15
 8048c24:	74 38                	je     8048c5e <L15>

08048c26 <L5>:

L5:
	subl $4,edi
 8048c26:	83 ef 04             	sub    $0x4,%edi
 8048c29:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

08048c30 <L10>:

/* loop performing 4 byte mask checking for desired 0 byte */
	.p2align 4,,7
L10:
	addl $4,edi
 8048c30:	83 c7 04             	add    $0x4,%edi
	movl (edi),ecx
 8048c33:	8b 0f                	mov    (%edi),%ecx
	leal -16843009(ecx),eax
 8048c35:	8d 81 ff fe fe fe    	lea    -0x1010101(%ecx),%eax
	notl ecx
 8048c3b:	f7 d1                	not    %ecx
	andl ecx,eax
 8048c3d:	21 c8                	and    %ecx,%eax
	testl $-2139062144,eax
 8048c3f:	a9 80 80 80 80       	test   $0x80808080,%eax
	je L10
 8048c44:	74 ea                	je     8048c30 <L10>

/* Find which of four bytes is 0.  */
	notl ecx
 8048c46:	f7 d1                	not    %ecx
	incl edi
 8048c48:	47                   	inc    %edi

	testb cl,cl
 8048c49:	84 c9                	test   %cl,%cl
	je L15
 8048c4b:	74 11                	je     8048c5e <L15>
	incl edi
 8048c4d:	47                   	inc    %edi
	shrl $8,ecx
 8048c4e:	c1 e9 08             	shr    $0x8,%ecx

	testb cl,cl
 8048c51:	84 c9                	test   %cl,%cl
	je L15
 8048c53:	74 09                	je     8048c5e <L15>
	incl edi
 8048c55:	47                   	inc    %edi
	shrl $8,ecx
 8048c56:	c1 e9 08             	shr    $0x8,%ecx

	testb cl,cl
 8048c59:	84 c9                	test   %cl,%cl
	je L15
 8048c5b:	74 01                	je     8048c5e <L15>
	incl edi
 8048c5d:	47                   	inc    %edi

08048c5e <L15>:

#endif

L15:
	subl edx,edi
 8048c5e:	29 d7                	sub    %edx,%edi
	leal -1(edi),eax
 8048c60:	8d 47 ff             	lea    -0x1(%edi),%eax

	leal -4(ebp),esp
 8048c63:	8d 65 fc             	lea    -0x4(%ebp),%esp
	popl edi
 8048c66:	5f                   	pop    %edi
	leave
 8048c67:	c9                   	leave  
	ret
 8048c68:	c3                   	ret    
 8048c69:	90                   	nop
 8048c6a:	90                   	nop
 8048c6b:	90                   	nop

08048c6c <_vfprintf_r>:
_DEFUN(_VFPRINTF_R, (data, fp, fmt0, ap),
       struct _reent *data _AND
       FILE * fp           _AND
       _CONST char *fmt0   _AND
       va_list ap)
{
 8048c6c:	55                   	push   %ebp
 8048c6d:	57                   	push   %edi
 8048c6e:	56                   	push   %esi
 8048c6f:	53                   	push   %ebx
 8048c70:	81 ec 4c 01 00 00    	sub    $0x14c,%esp
	char *thousands_sep = NULL;
	size_t thsnd_len = 0;
	const char *grouping = NULL;
#endif
#ifdef FLOATING_POINT
	char *decimal_point = _localeconv_r (data)->decimal_point;
 8048c76:	8b 84 24 60 01 00 00 	mov    0x160(%esp),%eax
 8048c7d:	89 04 24             	mov    %eax,(%esp)
 8048c80:	e8 a7 46 00 00       	call   804d32c <_localeconv_r>
 8048c85:	8b 00                	mov    (%eax),%eax
 8048c87:	89 44 24 74          	mov    %eax,0x74(%esp)
	size_t decp_len = strlen (decimal_point);
 8048c8b:	89 04 24             	mov    %eax,(%esp)
 8048c8e:	e8 5d ff ff ff       	call   8048bf0 <strlen>
 8048c93:	89 44 24 7c          	mov    %eax,0x7c(%esp)
	    (u_long)GET_ARG (N, ap, u_int))
#endif

#ifndef STRING_ONLY
	/* Initialize std streams if not dealing with sprintf family.  */
	CHECK_INIT (data, fp);
 8048c97:	8b bc 24 60 01 00 00 	mov    0x160(%esp),%edi
 8048c9e:	85 ff                	test   %edi,%edi
 8048ca0:	74 12                	je     8048cb4 <_vfprintf_r+0x48>
 8048ca2:	8b 84 24 60 01 00 00 	mov    0x160(%esp),%eax
 8048ca9:	8b 70 38             	mov    0x38(%eax),%esi
 8048cac:	85 f6                	test   %esi,%esi
 8048cae:	0f 84 78 06 00 00    	je     804932c <_vfprintf_r+0x6c0>
	_newlib_flockfile_start (fp);

	ORIENT(fp, -1);
 8048cb4:	8b 94 24 64 01 00 00 	mov    0x164(%esp),%edx
 8048cbb:	8b 42 0c             	mov    0xc(%edx),%eax
 8048cbe:	f6 c4 20             	test   $0x20,%ah
 8048cc1:	75 0e                	jne    8048cd1 <_vfprintf_r+0x65>
 8048cc3:	80 cc 20             	or     $0x20,%ah
 8048cc6:	66 89 42 0c          	mov    %ax,0xc(%edx)
 8048cca:	81 62 64 ff df ff ff 	andl   $0xffffdfff,0x64(%edx)

	/* sorry, fprintf(read_only_file, "") returns EOF, not 0 */
	if (cantwrite (data, fp)) {
 8048cd1:	a8 08                	test   $0x8,%al
 8048cd3:	0f 84 4b 05 00 00    	je     8049224 <_vfprintf_r+0x5b8>
 8048cd9:	8b 94 24 64 01 00 00 	mov    0x164(%esp),%edx
 8048ce0:	8b 5a 10             	mov    0x10(%edx),%ebx
 8048ce3:	85 db                	test   %ebx,%ebx
 8048ce5:	0f 84 39 05 00 00    	je     8049224 <_vfprintf_r+0x5b8>
		return (EOF);
	}

#ifdef _UNBUF_STREAM_OPT
	/* optimise fprintf(stderr) (and other unbuffered Unix files) */
	if ((fp->_flags & (__SNBF|__SWR|__SRW)) == (__SNBF|__SWR) &&
 8048ceb:	83 e0 1a             	and    $0x1a,%eax
 8048cee:	66 83 f8 0a          	cmp    $0xa,%ax
 8048cf2:	0f 84 65 05 00 00    	je     804925d <_vfprintf_r+0x5f1>
        }
#endif /* STRING_ONLY */

	fmt = (char *)fmt0;
#ifdef _FVWRITE_IN_STREAMIO
	uio.uio_iov = iovp = iov;
 8048cf8:	8d 9c 24 00 01 00 00 	lea    0x100(%esp),%ebx
 8048cff:	89 5c 24 44          	mov    %ebx,0x44(%esp)
 8048d03:	89 9c 24 cc 00 00 00 	mov    %ebx,0xcc(%esp)
	uio.uio_resid = 0;
 8048d0a:	c7 84 24 d4 00 00 00 	movl   $0x0,0xd4(%esp)
 8048d11:	00 00 00 00 
	uio.uio_iovcnt = 0;
 8048d15:	c7 84 24 d0 00 00 00 	movl   $0x0,0xd0(%esp)
 8048d1c:	00 00 00 00 
		}
		fp->_bf._size = 64;
        }
#endif /* STRING_ONLY */

	fmt = (char *)fmt0;
 8048d20:	8b 9c 24 68 01 00 00 	mov    0x168(%esp),%ebx
	int expsize = 0;	/* character count for expstr */
	char expstr[MAXEXPLEN];	/* buffer for exponent string */
	int lead;		/* sig figs before decimal or group sep */
#endif /* FLOATING_POINT */
#if defined (FLOATING_POINT) || defined (_WANT_IO_C99_FORMATS)
	int ndig = 0;		/* actual number of digits returned by cvt */
 8048d27:	c7 44 24 70 00 00 00 	movl   $0x0,0x70(%esp)
 8048d2e:	00 
#endif
#ifdef FLOATING_POINT
	char *decimal_point = _localeconv_r (data)->decimal_point;
	size_t decp_len = strlen (decimal_point);
	char softsign;		/* temporary negative sign for floats */
	union { int i; _PRINTF_FLOAT_TYPE fp; } _double_ = {0};
 8048d2f:	d9 ee                	fldz   
 8048d31:	dd 9c 24 80 00 00 00 	fstpl  0x80(%esp)
        }
#endif /* STRING_ONLY */

	fmt = (char *)fmt0;
#ifdef _FVWRITE_IN_STREAMIO
	uio.uio_iov = iovp = iov;
 8048d38:	8d bc 24 00 01 00 00 	lea    0x100(%esp),%edi
	u_quad_t _uquad;	/* integer arguments %[diouxX] */
	enum { OCT, DEC, HEX } base;/* base for [diouxX] conversion */
	int dprec;		/* a copy of prec if [diouxX], 0 otherwise */
	int realsz;		/* field size expanded by dprec */
	int size;		/* size of converted field or string */
	char *xdigs = NULL;	/* digits for [xX] conversion */
 8048d3f:	c7 84 24 8c 00 00 00 	movl   $0x0,0x8c(%esp)
 8048d46:	00 00 00 00 
	size_t decp_len = strlen (decimal_point);
	char softsign;		/* temporary negative sign for floats */
	union { int i; _PRINTF_FLOAT_TYPE fp; } _double_ = {0};
# define _fpvalue (_double_.fp)
	int expt;		/* integer value of exponent */
	int expsize = 0;	/* character count for expstr */
 8048d4a:	c7 84 24 88 00 00 00 	movl   $0x0,0x88(%esp)
 8048d51:	00 00 00 00 
#ifdef _FVWRITE_IN_STREAMIO
	uio.uio_iov = iovp = iov;
	uio.uio_resid = 0;
	uio.uio_iovcnt = 0;
#endif
	ret = 0;
 8048d55:	c7 44 24 54 00 00 00 	movl   $0x0,0x54(%esp)
 8048d5c:	00 
			*
			* To demonstrate this case, compile and run:
                        *    printf ("%#.0o",0);
			*/
                       else if (base == OCT && (flags & ALT))
                         *--cp = '0';
 8048d5d:	8d 8c 24 ff 00 00 00 	lea    0xff(%esp),%ecx
 8048d64:	89 4c 24 40          	mov    %ecx,0x40(%esp)
 8048d68:	89 f8                	mov    %edi,%eax
 8048d6a:	29 c8                	sub    %ecx,%eax
 8048d6c:	89 84 24 94 00 00 00 	mov    %eax,0x94(%esp)
                    else if (wc == '%')
                        break;
                    fmt += n;
		}
#else
                while (*fmt != '\0' && *fmt != '%')
 8048d73:	8a 03                	mov    (%ebx),%al
 8048d75:	89 de                	mov    %ebx,%esi
 8048d77:	84 c0                	test   %al,%al
 8048d79:	74 47                	je     8048dc2 <_vfprintf_r+0x156>
 8048d7b:	3c 25                	cmp    $0x25,%al
 8048d7d:	75 09                	jne    8048d88 <_vfprintf_r+0x11c>
 8048d7f:	eb 41                	jmp    8048dc2 <_vfprintf_r+0x156>
 8048d81:	8d 76 00             	lea    0x0(%esi),%esi
 8048d84:	3c 25                	cmp    $0x25,%al
 8048d86:	74 07                	je     8048d8f <_vfprintf_r+0x123>
                    fmt += 1;
 8048d88:	46                   	inc    %esi
                    else if (wc == '%')
                        break;
                    fmt += n;
		}
#else
                while (*fmt != '\0' && *fmt != '%')
 8048d89:	8a 06                	mov    (%esi),%al
 8048d8b:	84 c0                	test   %al,%al
 8048d8d:	75 f5                	jne    8048d84 <_vfprintf_r+0x118>
 8048d8f:	89 f5                	mov    %esi,%ebp
 8048d91:	29 dd                	sub    %ebx,%ebp
                    fmt += 1;
#endif
		if ((m = fmt - cp) != 0) {
 8048d93:	85 ed                	test   %ebp,%ebp
 8048d95:	74 2b                	je     8048dc2 <_vfprintf_r+0x156>
			PRINT (cp, m);
 8048d97:	89 1f                	mov    %ebx,(%edi)
 8048d99:	89 6f 04             	mov    %ebp,0x4(%edi)
 8048d9c:	01 ac 24 d4 00 00 00 	add    %ebp,0xd4(%esp)
 8048da3:	8b 94 24 d0 00 00 00 	mov    0xd0(%esp),%edx
 8048daa:	42                   	inc    %edx
 8048dab:	89 94 24 d0 00 00 00 	mov    %edx,0xd0(%esp)
 8048db2:	83 fa 07             	cmp    $0x7,%edx
 8048db5:	0f 8f 39 05 00 00    	jg     80492f4 <_vfprintf_r+0x688>
 8048dbb:	83 c7 08             	add    $0x8,%edi
			ret += m;
 8048dbe:	01 6c 24 54          	add    %ebp,0x54(%esp)
		}
#ifdef _MB_CAPABLE
		if (n <= 0)
                    goto done;
#else
                if (*fmt == '\0')
 8048dc2:	80 3e 00             	cmpb   $0x0,(%esi)
 8048dc5:	0f 84 d4 04 00 00    	je     804929f <_vfprintf_r+0x633>
                    goto done;
#endif
		fmt_anchor = fmt;
		fmt++;		/* skip over '%' */
 8048dcb:	8d 5e 01             	lea    0x1(%esi),%ebx

		flags = 0;
		dprec = 0;
		width = 0;
		prec = -1;
		sign = '\0';
 8048dce:	c6 84 24 ad 00 00 00 	movb   $0x0,0xad(%esp)
 8048dd5:	00 
 8048dd6:	31 c9                	xor    %ecx,%ecx
		fmt++;		/* skip over '%' */

		flags = 0;
		dprec = 0;
		width = 0;
		prec = -1;
 8048dd8:	bd ff ff ff ff       	mov    $0xffffffff,%ebp
		fmt_anchor = fmt;
		fmt++;		/* skip over '%' */

		flags = 0;
		dprec = 0;
		width = 0;
 8048ddd:	c7 44 24 5c 00 00 00 	movl   $0x0,0x5c(%esp)
 8048de4:	00 
                    goto done;
#endif
		fmt_anchor = fmt;
		fmt++;		/* skip over '%' */

		flags = 0;
 8048de5:	c7 44 24 48 00 00 00 	movl   $0x0,0x48(%esp)
 8048dec:	00 
#ifndef _NO_POS_ARGS
		N = arg_index;
		is_pos_arg = 0;
#endif

rflag:		ch = *fmt++;
 8048ded:	0f be 03             	movsbl (%ebx),%eax
 8048df0:	43                   	inc    %ebx
reswitch:	switch (ch) {
 8048df1:	8d 50 e0             	lea    -0x20(%eax),%edx
 8048df4:	83 fa 58             	cmp    $0x58,%edx
 8048df7:	0f 86 ff 03 00 00    	jbe    80491fc <_vfprintf_r+0x590>
 8048dfd:	89 44 24 68          	mov    %eax,0x68(%esp)
 8048e01:	88 8c 24 ad 00 00 00 	mov    %cl,0xad(%esp)

			size = buf + BUF - cp;
		skipsize:
			break;
		default:	/* "%?" prints ?, unless ? is NUL */
			if (ch == '\0')
 8048e08:	85 c0                	test   %eax,%eax
 8048e0a:	0f 84 8f 04 00 00    	je     804929f <_vfprintf_r+0x633>
				goto done;
			/* pretend it was %c with argument ch */
			cp = buf;
			*cp = ch;
 8048e10:	88 84 24 d8 00 00 00 	mov    %al,0xd8(%esp)
			size = 1;
			sign = '\0';
 8048e17:	c6 84 24 ad 00 00 00 	movb   $0x0,0xad(%esp)
 8048e1e:	00 
			break;
 8048e1f:	31 f6                	xor    %esi,%esi
 8048e21:	c7 44 24 4c 01 00 00 	movl   $0x1,0x4c(%esp)
 8048e28:	00 
			if (ch == '\0')
				goto done;
			/* pretend it was %c with argument ch */
			cp = buf;
			*cp = ch;
			size = 1;
 8048e29:	c7 44 24 50 01 00 00 	movl   $0x1,0x50(%esp)
 8048e30:	00 
#endif
		fmt_anchor = fmt;
		fmt++;		/* skip over '%' */

		flags = 0;
		dprec = 0;
 8048e31:	c7 44 24 6c 00 00 00 	movl   $0x0,0x6c(%esp)
 8048e38:	00 
		width = 0;
		prec = -1;
		sign = '\0';
#ifdef FLOATING_POINT
		lead = 0;
 8048e39:	c7 44 24 78 00 00 00 	movl   $0x0,0x78(%esp)
 8048e40:	00 
			break;
		default:	/* "%?" prints ?, unless ? is NUL */
			if (ch == '\0')
				goto done;
			/* pretend it was %c with argument ch */
			cp = buf;
 8048e41:	8d 84 24 d8 00 00 00 	lea    0xd8(%esp),%eax
 8048e48:	89 44 24 60          	mov    %eax,0x60(%esp)
		 * size excludes decimal prec; realsz includes it.
		 */
		realsz = dprec > size ? dprec : size;
		if (sign)
			realsz++;
		if (flags & HEXPREFIX)
 8048e4c:	8b 4c 24 48          	mov    0x48(%esp),%ecx
 8048e50:	83 e1 02             	and    $0x2,%ecx
 8048e53:	89 4c 24 58          	mov    %ecx,0x58(%esp)
 8048e57:	74 05                	je     8048e5e <_vfprintf_r+0x1f2>
			realsz+= 2;
 8048e59:	83 44 24 4c 02       	addl   $0x2,0x4c(%esp)

		/* right-adjusting blank padding */
		if ((flags & (LADJUST|ZEROPAD)) == 0)
 8048e5e:	8b 4c 24 48          	mov    0x48(%esp),%ecx
 8048e62:	81 e1 84 00 00 00    	and    $0x84,%ecx
 8048e68:	89 4c 24 64          	mov    %ecx,0x64(%esp)
 8048e6c:	0f 85 d2 00 00 00    	jne    8048f44 <_vfprintf_r+0x2d8>
			PAD (width - realsz, blanks);
 8048e72:	8b 6c 24 5c          	mov    0x5c(%esp),%ebp
 8048e76:	2b 6c 24 4c          	sub    0x4c(%esp),%ebp
 8048e7a:	85 ed                	test   %ebp,%ebp
 8048e7c:	0f 8e c2 00 00 00    	jle    8048f44 <_vfprintf_r+0x2d8>
 8048e82:	8b 94 24 d0 00 00 00 	mov    0xd0(%esp),%edx
 8048e89:	83 fd 10             	cmp    $0x10,%ebp
 8048e8c:	0f 8e 86 00 00 00    	jle    8048f18 <_vfprintf_r+0x2ac>
 8048e92:	8d b4 24 cc 00 00 00 	lea    0xcc(%esp),%esi
 8048e99:	89 9c 24 90 00 00 00 	mov    %ebx,0x90(%esp)
 8048ea0:	89 f8                	mov    %edi,%eax
 8048ea2:	8b 9c 24 60 01 00 00 	mov    0x160(%esp),%ebx
 8048ea9:	8b bc 24 64 01 00 00 	mov    0x164(%esp),%edi
 8048eb0:	eb 0d                	jmp    8048ebf <_vfprintf_r+0x253>
 8048eb2:	66 90                	xchg   %ax,%ax
 8048eb4:	83 c0 08             	add    $0x8,%eax
 8048eb7:	83 ed 10             	sub    $0x10,%ebp
 8048eba:	83 fd 10             	cmp    $0x10,%ebp
 8048ebd:	7e 50                	jle    8048f0f <_vfprintf_r+0x2a3>
 8048ebf:	c7 00 34 18 05 08    	movl   $0x8051834,(%eax)
 8048ec5:	c7 40 04 10 00 00 00 	movl   $0x10,0x4(%eax)
 8048ecc:	83 84 24 d4 00 00 00 	addl   $0x10,0xd4(%esp)
 8048ed3:	10 
 8048ed4:	42                   	inc    %edx
 8048ed5:	89 94 24 d0 00 00 00 	mov    %edx,0xd0(%esp)
 8048edc:	83 fa 07             	cmp    $0x7,%edx
 8048edf:	7e d3                	jle    8048eb4 <_vfprintf_r+0x248>
 8048ee1:	89 74 24 08          	mov    %esi,0x8(%esp)
 8048ee5:	89 7c 24 04          	mov    %edi,0x4(%esp)
 8048ee9:	89 1c 24             	mov    %ebx,(%esp)
 8048eec:	e8 fb 63 00 00       	call   804f2ec <__sprint_r>
 8048ef1:	85 c0                	test   %eax,%eax
 8048ef3:	0f 85 d7 03 00 00    	jne    80492d0 <_vfprintf_r+0x664>
 8048ef9:	8d 84 24 00 01 00 00 	lea    0x100(%esp),%eax
 8048f00:	8b 94 24 d0 00 00 00 	mov    0xd0(%esp),%edx
 8048f07:	83 ed 10             	sub    $0x10,%ebp
 8048f0a:	83 fd 10             	cmp    $0x10,%ebp
 8048f0d:	7f b0                	jg     8048ebf <_vfprintf_r+0x253>
 8048f0f:	8b 9c 24 90 00 00 00 	mov    0x90(%esp),%ebx
 8048f16:	89 c7                	mov    %eax,%edi
 8048f18:	c7 07 34 18 05 08    	movl   $0x8051834,(%edi)
 8048f1e:	89 6f 04             	mov    %ebp,0x4(%edi)
 8048f21:	01 ac 24 d4 00 00 00 	add    %ebp,0xd4(%esp)
 8048f28:	42                   	inc    %edx
 8048f29:	89 94 24 d0 00 00 00 	mov    %edx,0xd0(%esp)
 8048f30:	83 fa 07             	cmp    $0x7,%edx
 8048f33:	0f 8f 38 0f 00 00    	jg     8049e71 <_vfprintf_r+0x1205>
 8048f39:	83 c7 08             	add    $0x8,%edi
 8048f3c:	0f b6 b4 24 ad 00 00 	movzbl 0xad(%esp),%esi
 8048f43:	00 

		/* prefix */
		if (sign)
 8048f44:	89 f2                	mov    %esi,%edx
 8048f46:	84 d2                	test   %dl,%dl
 8048f48:	74 32                	je     8048f7c <_vfprintf_r+0x310>
			PRINT (&sign, 1);
 8048f4a:	8d 84 24 ad 00 00 00 	lea    0xad(%esp),%eax
 8048f51:	89 07                	mov    %eax,(%edi)
 8048f53:	c7 47 04 01 00 00 00 	movl   $0x1,0x4(%edi)
 8048f5a:	ff 84 24 d4 00 00 00 	incl   0xd4(%esp)
 8048f61:	8b 94 24 d0 00 00 00 	mov    0xd0(%esp),%edx
 8048f68:	42                   	inc    %edx
 8048f69:	89 94 24 d0 00 00 00 	mov    %edx,0xd0(%esp)
 8048f70:	83 fa 07             	cmp    $0x7,%edx
 8048f73:	0f 8f e7 0c 00 00    	jg     8049c60 <_vfprintf_r+0xff4>
 8048f79:	83 c7 08             	add    $0x8,%edi
		if (flags & HEXPREFIX)
 8048f7c:	8b 4c 24 58          	mov    0x58(%esp),%ecx
 8048f80:	85 c9                	test   %ecx,%ecx
 8048f82:	74 33                	je     8048fb7 <_vfprintf_r+0x34b>
			PRINT (ox, 2);
 8048f84:	8d 84 24 ae 00 00 00 	lea    0xae(%esp),%eax
 8048f8b:	89 07                	mov    %eax,(%edi)
 8048f8d:	c7 47 04 02 00 00 00 	movl   $0x2,0x4(%edi)
 8048f94:	83 84 24 d4 00 00 00 	addl   $0x2,0xd4(%esp)
 8048f9b:	02 
 8048f9c:	8b 94 24 d0 00 00 00 	mov    0xd0(%esp),%edx
 8048fa3:	42                   	inc    %edx
 8048fa4:	89 94 24 d0 00 00 00 	mov    %edx,0xd0(%esp)
 8048fab:	83 fa 07             	cmp    $0x7,%edx
 8048fae:	0f 8f e8 0c 00 00    	jg     8049c9c <_vfprintf_r+0x1030>
 8048fb4:	83 c7 08             	add    $0x8,%edi

		/* right-adjusting zero padding */
		if ((flags & (LADJUST|ZEROPAD)) == ZEROPAD)
 8048fb7:	81 7c 24 64 80 00 00 	cmpl   $0x80,0x64(%esp)
 8048fbe:	00 
 8048fbf:	0f 84 47 0a 00 00    	je     8049a0c <_vfprintf_r+0xda0>
			PAD (width - realsz, zeroes);

		/* leading zeroes from decimal precision */
		PAD (dprec - size, zeroes);
 8048fc5:	8b 6c 24 6c          	mov    0x6c(%esp),%ebp
 8048fc9:	2b 6c 24 50          	sub    0x50(%esp),%ebp
 8048fcd:	85 ed                	test   %ebp,%ebp
 8048fcf:	0f 8e b4 00 00 00    	jle    8049089 <_vfprintf_r+0x41d>
 8048fd5:	8b 94 24 d0 00 00 00 	mov    0xd0(%esp),%edx
 8048fdc:	83 fd 10             	cmp    $0x10,%ebp
 8048fdf:	0f 8e 80 00 00 00    	jle    8049065 <_vfprintf_r+0x3f9>
 8048fe5:	8d b4 24 cc 00 00 00 	lea    0xcc(%esp),%esi
 8048fec:	89 5c 24 58          	mov    %ebx,0x58(%esp)
 8048ff0:	89 f8                	mov    %edi,%eax
 8048ff2:	8b 9c 24 60 01 00 00 	mov    0x160(%esp),%ebx
 8048ff9:	8b bc 24 64 01 00 00 	mov    0x164(%esp),%edi
 8049000:	eb 0d                	jmp    804900f <_vfprintf_r+0x3a3>
 8049002:	66 90                	xchg   %ax,%ax
 8049004:	83 c0 08             	add    $0x8,%eax
 8049007:	83 ed 10             	sub    $0x10,%ebp
 804900a:	83 fd 10             	cmp    $0x10,%ebp
 804900d:	7e 50                	jle    804905f <_vfprintf_r+0x3f3>
 804900f:	c7 00 44 18 05 08    	movl   $0x8051844,(%eax)
 8049015:	c7 40 04 10 00 00 00 	movl   $0x10,0x4(%eax)
 804901c:	83 84 24 d4 00 00 00 	addl   $0x10,0xd4(%esp)
 8049023:	10 
 8049024:	42                   	inc    %edx
 8049025:	89 94 24 d0 00 00 00 	mov    %edx,0xd0(%esp)
 804902c:	83 fa 07             	cmp    $0x7,%edx
 804902f:	7e d3                	jle    8049004 <_vfprintf_r+0x398>
 8049031:	89 74 24 08          	mov    %esi,0x8(%esp)
 8049035:	89 7c 24 04          	mov    %edi,0x4(%esp)
 8049039:	89 1c 24             	mov    %ebx,(%esp)
 804903c:	e8 ab 62 00 00       	call   804f2ec <__sprint_r>
 8049041:	85 c0                	test   %eax,%eax
 8049043:	0f 85 87 02 00 00    	jne    80492d0 <_vfprintf_r+0x664>
 8049049:	8d 84 24 00 01 00 00 	lea    0x100(%esp),%eax
 8049050:	8b 94 24 d0 00 00 00 	mov    0xd0(%esp),%edx
 8049057:	83 ed 10             	sub    $0x10,%ebp
 804905a:	83 fd 10             	cmp    $0x10,%ebp
 804905d:	7f b0                	jg     804900f <_vfprintf_r+0x3a3>
 804905f:	8b 5c 24 58          	mov    0x58(%esp),%ebx
 8049063:	89 c7                	mov    %eax,%edi
 8049065:	c7 07 44 18 05 08    	movl   $0x8051844,(%edi)
 804906b:	89 6f 04             	mov    %ebp,0x4(%edi)
 804906e:	01 ac 24 d4 00 00 00 	add    %ebp,0xd4(%esp)
 8049075:	42                   	inc    %edx
 8049076:	89 94 24 d0 00 00 00 	mov    %edx,0xd0(%esp)
 804907d:	83 fa 07             	cmp    $0x7,%edx
 8049080:	0f 8f 9e 0b 00 00    	jg     8049c24 <_vfprintf_r+0xfb8>
 8049086:	83 c7 08             	add    $0x8,%edi

		/* the string or number proper */
#ifdef FLOATING_POINT
		if ((flags & FPT) == 0) {
 8049089:	f7 44 24 48 00 01 00 	testl  $0x100,0x48(%esp)
 8049090:	00 
 8049091:	0f 85 41 08 00 00    	jne    80498d8 <_vfprintf_r+0xc6c>
			PRINT (cp, size);
 8049097:	8b 44 24 60          	mov    0x60(%esp),%eax
 804909b:	89 07                	mov    %eax,(%edi)
 804909d:	8b 44 24 50          	mov    0x50(%esp),%eax
 80490a1:	89 47 04             	mov    %eax,0x4(%edi)
 80490a4:	01 84 24 d4 00 00 00 	add    %eax,0xd4(%esp)
 80490ab:	8b 94 24 d0 00 00 00 	mov    0xd0(%esp),%edx
 80490b2:	42                   	inc    %edx
 80490b3:	89 94 24 d0 00 00 00 	mov    %edx,0xd0(%esp)
 80490ba:	83 fa 07             	cmp    $0x7,%edx
 80490bd:	0f 8f f4 0a 00 00    	jg     8049bb7 <_vfprintf_r+0xf4b>
					} else	/* 0.[0..] */
						/* __dtoa irregularity */
						PAD (ndig - 1, zeroes);
				} else	/* XeYYY */
					PRINT (cp, 1);
				PRINT (expstr, expsize);
 80490c3:	83 c7 08             	add    $0x8,%edi
		}
#else /* !FLOATING_POINT */
		PRINT (cp, size);
#endif
		/* left-adjusting padding (always blank) */
		if (flags & LADJUST)
 80490c6:	f6 44 24 48 04       	testb  $0x4,0x48(%esp)
 80490cb:	0f 84 f3 00 00 00    	je     80491c4 <_vfprintf_r+0x558>
			PAD (width - realsz, blanks);
 80490d1:	8b 6c 24 5c          	mov    0x5c(%esp),%ebp
 80490d5:	2b 6c 24 4c          	sub    0x4c(%esp),%ebp
 80490d9:	85 ed                	test   %ebp,%ebp
 80490db:	0f 8e e3 00 00 00    	jle    80491c4 <_vfprintf_r+0x558>
 80490e1:	8b 94 24 d0 00 00 00 	mov    0xd0(%esp),%edx
 80490e8:	83 fd 10             	cmp    $0x10,%ebp
 80490eb:	0f 8e 80 00 00 00    	jle    8049171 <_vfprintf_r+0x505>
 80490f1:	8d b4 24 cc 00 00 00 	lea    0xcc(%esp),%esi
 80490f8:	89 5c 24 48          	mov    %ebx,0x48(%esp)
 80490fc:	89 f8                	mov    %edi,%eax
 80490fe:	8b 9c 24 60 01 00 00 	mov    0x160(%esp),%ebx
 8049105:	8b bc 24 64 01 00 00 	mov    0x164(%esp),%edi
 804910c:	eb 0d                	jmp    804911b <_vfprintf_r+0x4af>
 804910e:	66 90                	xchg   %ax,%ax
 8049110:	83 c0 08             	add    $0x8,%eax
 8049113:	83 ed 10             	sub    $0x10,%ebp
 8049116:	83 fd 10             	cmp    $0x10,%ebp
 8049119:	7e 50                	jle    804916b <_vfprintf_r+0x4ff>
 804911b:	c7 00 34 18 05 08    	movl   $0x8051834,(%eax)
 8049121:	c7 40 04 10 00 00 00 	movl   $0x10,0x4(%eax)
 8049128:	83 84 24 d4 00 00 00 	addl   $0x10,0xd4(%esp)
 804912f:	10 
 8049130:	42                   	inc    %edx
 8049131:	89 94 24 d0 00 00 00 	mov    %edx,0xd0(%esp)
 8049138:	83 fa 07             	cmp    $0x7,%edx
 804913b:	7e d3                	jle    8049110 <_vfprintf_r+0x4a4>
 804913d:	89 74 24 08          	mov    %esi,0x8(%esp)
 8049141:	89 7c 24 04          	mov    %edi,0x4(%esp)
 8049145:	89 1c 24             	mov    %ebx,(%esp)
 8049148:	e8 9f 61 00 00       	call   804f2ec <__sprint_r>
 804914d:	85 c0                	test   %eax,%eax
 804914f:	0f 85 7b 01 00 00    	jne    80492d0 <_vfprintf_r+0x664>
 8049155:	8d 84 24 00 01 00 00 	lea    0x100(%esp),%eax
 804915c:	8b 94 24 d0 00 00 00 	mov    0xd0(%esp),%edx
 8049163:	83 ed 10             	sub    $0x10,%ebp
 8049166:	83 fd 10             	cmp    $0x10,%ebp
 8049169:	7f b0                	jg     804911b <_vfprintf_r+0x4af>
 804916b:	8b 5c 24 48          	mov    0x48(%esp),%ebx
 804916f:	89 c7                	mov    %eax,%edi
 8049171:	c7 07 34 18 05 08    	movl   $0x8051834,(%edi)
 8049177:	89 6f 04             	mov    %ebp,0x4(%edi)
 804917a:	03 ac 24 d4 00 00 00 	add    0xd4(%esp),%ebp
 8049181:	89 ac 24 d4 00 00 00 	mov    %ebp,0xd4(%esp)
 8049188:	42                   	inc    %edx
 8049189:	89 94 24 d0 00 00 00 	mov    %edx,0xd0(%esp)
 8049190:	83 fa 07             	cmp    $0x7,%edx
 8049193:	7e 36                	jle    80491cb <_vfprintf_r+0x55f>
 8049195:	8d 84 24 cc 00 00 00 	lea    0xcc(%esp),%eax
 804919c:	89 44 24 08          	mov    %eax,0x8(%esp)
 80491a0:	8b 94 24 64 01 00 00 	mov    0x164(%esp),%edx
 80491a7:	89 54 24 04          	mov    %edx,0x4(%esp)
 80491ab:	8b 84 24 60 01 00 00 	mov    0x160(%esp),%eax
 80491b2:	89 04 24             	mov    %eax,(%esp)
 80491b5:	e8 32 61 00 00       	call   804f2ec <__sprint_r>
 80491ba:	85 c0                	test   %eax,%eax
 80491bc:	0f 85 0e 01 00 00    	jne    80492d0 <_vfprintf_r+0x664>
 80491c2:	66 90                	xchg   %ax,%ax
 80491c4:	8b ac 24 d4 00 00 00 	mov    0xd4(%esp),%ebp

		/* finally, adjust ret */
		ret += width > realsz ? width : realsz;
 80491cb:	8b 44 24 4c          	mov    0x4c(%esp),%eax
 80491cf:	3b 44 24 5c          	cmp    0x5c(%esp),%eax
 80491d3:	7d 04                	jge    80491d9 <_vfprintf_r+0x56d>
 80491d5:	8b 44 24 5c          	mov    0x5c(%esp),%eax
 80491d9:	01 44 24 54          	add    %eax,0x54(%esp)

		FLUSH ();	/* copy out the I/O vectors */
 80491dd:	85 ed                	test   %ebp,%ebp
 80491df:	0f 85 0b 0a 00 00    	jne    8049bf0 <_vfprintf_r+0xf84>
 80491e5:	c7 84 24 d0 00 00 00 	movl   $0x0,0xd0(%esp)
 80491ec:	00 00 00 00 
 80491f0:	8d bc 24 00 01 00 00 	lea    0x100(%esp),%edi
 80491f7:	e9 77 fb ff ff       	jmp    8048d73 <_vfprintf_r+0x107>
		N = arg_index;
		is_pos_arg = 0;
#endif

rflag:		ch = *fmt++;
reswitch:	switch (ch) {
 80491fc:	ff 24 95 d0 16 05 08 	jmp    *0x80516d0(,%edx,4)
			} else
#endif
				flags |= LONGINT;
			goto rflag;
		case 'q': /* extension */
			flags |= QUADINT;
 8049203:	83 4c 24 48 10       	orl    $0x10,0x48(%esp)
			goto rflag;
 8049208:	e9 e0 fb ff ff       	jmp    8048ded <_vfprintf_r+0x181>
#ifndef _NO_POS_ARGS
			is_pos_arg = old_is_pos_arg;
#endif
			if (width >= 0)
				goto rflag;
			width = -width;
 804920d:	f7 da                	neg    %edx
 804920f:	89 54 24 5c          	mov    %edx,0x5c(%esp)
			 * ``A negative field width argument is taken as a
			 * - flag followed by a positive field width.''
			 *	-- ANSI X3J11
			 * They don't exclude field widths read from args.
			 */
			width = GET_ARG (n, ap, int);
 8049213:	89 84 24 6c 01 00 00 	mov    %eax,0x16c(%esp)
			if (width >= 0)
				goto rflag;
			width = -width;
			/* FALLTHROUGH */
		case '-':
			flags |= LADJUST;
 804921a:	83 4c 24 48 04       	orl    $0x4,0x48(%esp)
			goto rflag;
 804921f:	e9 c9 fb ff ff       	jmp    8048ded <_vfprintf_r+0x181>
	_newlib_flockfile_start (fp);

	ORIENT(fp, -1);

	/* sorry, fprintf(read_only_file, "") returns EOF, not 0 */
	if (cantwrite (data, fp)) {
 8049224:	8b 94 24 64 01 00 00 	mov    0x164(%esp),%edx
 804922b:	89 54 24 04          	mov    %edx,0x4(%esp)
 804922f:	8b 84 24 60 01 00 00 	mov    0x160(%esp),%eax
 8049236:	89 04 24             	mov    %eax,(%esp)
 8049239:	e8 7a 1b 00 00       	call   804adb8 <__swsetup_r>
 804923e:	85 c0                	test   %eax,%eax
 8049240:	0f 85 97 00 00 00    	jne    80492dd <_vfprintf_r+0x671>
 8049246:	8b 94 24 64 01 00 00 	mov    0x164(%esp),%edx
 804924d:	8b 42 0c             	mov    0xc(%edx),%eax
		return (EOF);
	}

#ifdef _UNBUF_STREAM_OPT
	/* optimise fprintf(stderr) (and other unbuffered Unix files) */
	if ((fp->_flags & (__SNBF|__SWR|__SRW)) == (__SNBF|__SWR) &&
 8049250:	83 e0 1a             	and    $0x1a,%eax
 8049253:	66 83 f8 0a          	cmp    $0xa,%ax
 8049257:	0f 85 9b fa ff ff    	jne    8048cf8 <_vfprintf_r+0x8c>
 804925d:	8b 94 24 64 01 00 00 	mov    0x164(%esp),%edx
 8049264:	66 83 7a 0e 00       	cmpw   $0x0,0xe(%edx)
 8049269:	0f 88 89 fa ff ff    	js     8048cf8 <_vfprintf_r+0x8c>
	    fp->_file >= 0) {
		_newlib_flockfile_exit (fp);
		return (__sbprintf (data, fp, fmt0, ap));
 804926f:	8b 8c 24 6c 01 00 00 	mov    0x16c(%esp),%ecx
 8049276:	89 0c 24             	mov    %ecx,(%esp)
 8049279:	8b 8c 24 68 01 00 00 	mov    0x168(%esp),%ecx
 8049280:	8b 84 24 60 01 00 00 	mov    0x160(%esp),%eax
 8049287:	e8 4c 1a 00 00       	call   804acd8 <__sbprintf>
 804928c:	89 44 24 54          	mov    %eax,0x54(%esp)
#ifndef STRING_ONLY
	_newlib_flockfile_end (fp);
#endif
	return (__sferror (fp) ? EOF : ret);
	/* NOTREACHED */
}
 8049290:	8b 44 24 54          	mov    0x54(%esp),%eax
 8049294:	81 c4 4c 01 00 00    	add    $0x14c,%esp
 804929a:	5b                   	pop    %ebx
 804929b:	5e                   	pop    %esi
 804929c:	5f                   	pop    %edi
 804929d:	5d                   	pop    %ebp
 804929e:	c3                   	ret    
			_free_r (data, malloc_buf);
			malloc_buf = NULL;
		}
	}
done:
	FLUSH ();
 804929f:	8b 84 24 d4 00 00 00 	mov    0xd4(%esp),%eax
 80492a6:	85 c0                	test   %eax,%eax
 80492a8:	74 26                	je     80492d0 <_vfprintf_r+0x664>
 80492aa:	8d 84 24 cc 00 00 00 	lea    0xcc(%esp),%eax
 80492b1:	89 44 24 08          	mov    %eax,0x8(%esp)
 80492b5:	8b 94 24 64 01 00 00 	mov    0x164(%esp),%edx
 80492bc:	89 54 24 04          	mov    %edx,0x4(%esp)
 80492c0:	8b 84 24 60 01 00 00 	mov    0x160(%esp),%eax
 80492c7:	89 04 24             	mov    %eax,(%esp)
 80492ca:	e8 1d 60 00 00       	call   804f2ec <__sprint_r>
 80492cf:	90                   	nop
	if (malloc_buf != NULL)
		_free_r (data, malloc_buf);
#ifndef STRING_ONLY
	_newlib_flockfile_end (fp);
#endif
	return (__sferror (fp) ? EOF : ret);
 80492d0:	8b 94 24 64 01 00 00 	mov    0x164(%esp),%edx
 80492d7:	f6 42 0c 40          	testb  $0x40,0xc(%edx)
 80492db:	74 08                	je     80492e5 <_vfprintf_r+0x679>
	ORIENT(fp, -1);

	/* sorry, fprintf(read_only_file, "") returns EOF, not 0 */
	if (cantwrite (data, fp)) {
		_newlib_flockfile_exit (fp);
		return (EOF);
 80492dd:	c7 44 24 54 ff ff ff 	movl   $0xffffffff,0x54(%esp)
 80492e4:	ff 
#ifndef STRING_ONLY
	_newlib_flockfile_end (fp);
#endif
	return (__sferror (fp) ? EOF : ret);
	/* NOTREACHED */
}
 80492e5:	8b 44 24 54          	mov    0x54(%esp),%eax
 80492e9:	81 c4 4c 01 00 00    	add    $0x14c,%esp
 80492ef:	5b                   	pop    %ebx
 80492f0:	5e                   	pop    %esi
 80492f1:	5f                   	pop    %edi
 80492f2:	5d                   	pop    %ebp
 80492f3:	c3                   	ret    
#else
                while (*fmt != '\0' && *fmt != '%')
                    fmt += 1;
#endif
		if ((m = fmt - cp) != 0) {
			PRINT (cp, m);
 80492f4:	8d 84 24 cc 00 00 00 	lea    0xcc(%esp),%eax
 80492fb:	89 44 24 08          	mov    %eax,0x8(%esp)
 80492ff:	8b 94 24 64 01 00 00 	mov    0x164(%esp),%edx
 8049306:	89 54 24 04          	mov    %edx,0x4(%esp)
 804930a:	8b 84 24 60 01 00 00 	mov    0x160(%esp),%eax
 8049311:	89 04 24             	mov    %eax,(%esp)
 8049314:	e8 d3 5f 00 00       	call   804f2ec <__sprint_r>
 8049319:	85 c0                	test   %eax,%eax
 804931b:	75 b3                	jne    80492d0 <_vfprintf_r+0x664>
 804931d:	8d bc 24 00 01 00 00 	lea    0x100(%esp),%edi
 8049324:	e9 95 fa ff ff       	jmp    8048dbe <_vfprintf_r+0x152>
 8049329:	8d 76 00             	lea    0x0(%esi),%esi
	    (u_long)GET_ARG (N, ap, u_int))
#endif

#ifndef STRING_ONLY
	/* Initialize std streams if not dealing with sprintf family.  */
	CHECK_INIT (data, fp);
 804932c:	89 04 24             	mov    %eax,(%esp)
 804932f:	e8 30 35 00 00       	call   804c864 <__sinit>
 8049334:	e9 7b f9 ff ff       	jmp    8048cb4 <_vfprintf_r+0x48>
 8049339:	89 44 24 68          	mov    %eax,0x68(%esp)
			goto nosign;
		case 's':
#ifdef _WANT_IO_C99_FORMATS
		case 'S':
#endif
			cp = GET_ARG (N, ap, char_ptr_t);
 804933d:	8b 94 24 6c 01 00 00 	mov    0x16c(%esp),%edx
 8049344:	83 c2 04             	add    $0x4,%edx
 8049347:	8b 8c 24 6c 01 00 00 	mov    0x16c(%esp),%ecx
 804934e:	8b 09                	mov    (%ecx),%ecx
 8049350:	89 4c 24 60          	mov    %ecx,0x60(%esp)
#ifdef _GLIBC_EXTENSION
string:
#endif
			sign = '\0';
 8049354:	c6 84 24 ad 00 00 00 	movb   $0x0,0xad(%esp)
 804935b:	00 
#ifndef __OPTIMIZE_SIZE__
			/* Behavior is undefined if the user passed a
			   NULL string when precision is not 0.
			   However, if we are not optimizing for size,
			   we might as well mirror glibc behavior.  */
			if (cp == NULL) {
 804935c:	85 c9                	test   %ecx,%ecx
 804935e:	0f 84 34 15 00 00    	je     804a898 <_vfprintf_r+0x1c2c>
				}
				cp[size] = '\0';
			}
			else
#endif /* _MB_CAPABLE */
			if (prec >= 0) {
 8049364:	85 ed                	test   %ebp,%ebp
 8049366:	0f 88 b8 13 00 00    	js     804a724 <_vfprintf_r+0x1ab8>
				/*
				 * can't use strlen; can only look for the
				 * NUL in the first `prec' characters, and
				 * strlen () will go further.
				 */
				char *p = memchr (cp, 0, prec);
 804936c:	89 6c 24 08          	mov    %ebp,0x8(%esp)
 8049370:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
 8049377:	00 
 8049378:	8b 44 24 60          	mov    0x60(%esp),%eax
 804937c:	89 04 24             	mov    %eax,(%esp)
 804937f:	89 54 24 20          	mov    %edx,0x20(%esp)
 8049383:	e8 28 48 00 00       	call   804dbb0 <memchr>

				if (p != NULL) {
 8049388:	85 c0                	test   %eax,%eax
 804938a:	8b 54 24 20          	mov    0x20(%esp),%edx
 804938e:	0f 84 c9 04 00 00    	je     804985d <_vfprintf_r+0xbf1>
					size = p - cp;
 8049394:	2b 44 24 60          	sub    0x60(%esp),%eax
 8049398:	89 44 24 50          	mov    %eax,0x50(%esp)
					if (size > prec)
 804939c:	39 e8                	cmp    %ebp,%eax
 804939e:	0f 8f b9 04 00 00    	jg     804985d <_vfprintf_r+0xbf1>
 80493a4:	8b 44 24 50          	mov    0x50(%esp),%eax
 80493a8:	89 44 24 4c          	mov    %eax,0x4c(%esp)
 80493ac:	85 c0                	test   %eax,%eax
 80493ae:	0f 88 94 13 00 00    	js     804a748 <_vfprintf_r+0x1adc>
 80493b4:	0f b6 b4 24 ad 00 00 	movzbl 0xad(%esp),%esi
 80493bb:	00 
			goto nosign;
		case 's':
#ifdef _WANT_IO_C99_FORMATS
		case 'S':
#endif
			cp = GET_ARG (N, ap, char_ptr_t);
 80493bc:	89 94 24 6c 01 00 00 	mov    %edx,0x16c(%esp)
#endif
		fmt_anchor = fmt;
		fmt++;		/* skip over '%' */

		flags = 0;
		dprec = 0;
 80493c3:	c7 44 24 6c 00 00 00 	movl   $0x0,0x6c(%esp)
 80493ca:	00 
		width = 0;
		prec = -1;
		sign = '\0';
#ifdef FLOATING_POINT
		lead = 0;
 80493cb:	c7 44 24 78 00 00 00 	movl   $0x0,0x78(%esp)
 80493d2:	00 
 80493d3:	e9 b4 00 00 00       	jmp    804948c <_vfprintf_r+0x820>
			 * of printable characters, in an implementation-
			 * defined manner.''
			 *	-- ANSI X3J11
			 */
			/* NOSTRICT */
			_uquad = (uintptr_t) GET_ARG (N, ap, void_ptr_t);
 80493d8:	8b 84 24 6c 01 00 00 	mov    0x16c(%esp),%eax
 80493df:	8b 08                	mov    (%eax),%ecx
			base = HEX;
			xdigs = "0123456789abcdef";
			flags |= HEXPREFIX;
 80493e1:	83 4c 24 48 02       	orl    $0x2,0x48(%esp)
			ox[0] = '0';
 80493e6:	c6 84 24 ae 00 00 00 	movb   $0x30,0xae(%esp)
 80493ed:	30 
			ox[1] = ch = 'x';
 80493ee:	c6 84 24 af 00 00 00 	movb   $0x78,0xaf(%esp)
 80493f5:	78 
			 * of printable characters, in an implementation-
			 * defined manner.''
			 *	-- ANSI X3J11
			 */
			/* NOSTRICT */
			_uquad = (uintptr_t) GET_ARG (N, ap, void_ptr_t);
 80493f6:	83 c0 04             	add    $0x4,%eax
 80493f9:	89 84 24 6c 01 00 00 	mov    %eax,0x16c(%esp)
			base = HEX;
			xdigs = "0123456789abcdef";
 8049400:	c7 84 24 8c 00 00 00 	movl   $0x80516b5,0x8c(%esp)
 8049407:	b5 16 05 08 
			 * defined manner.''
			 *	-- ANSI X3J11
			 */
			/* NOSTRICT */
			_uquad = (uintptr_t) GET_ARG (N, ap, void_ptr_t);
			base = HEX;
 804940b:	b0 02                	mov    $0x2,%al
			xdigs = "0123456789abcdef";
			flags |= HEXPREFIX;
			ox[0] = '0';
			ox[1] = ch = 'x';
 804940d:	c7 44 24 68 78 00 00 	movl   $0x78,0x68(%esp)
 8049414:	00 
 8049415:	8d 76 00             	lea    0x0(%esi),%esi

#ifdef _WANT_IO_C99_FORMATS
			flags &= ~GROUPING;
#endif
			/* unsigned conversions */
nosign:			sign = '\0';
 8049418:	c6 84 24 ad 00 00 00 	movb   $0x0,0xad(%esp)
 804941f:	00 
 8049420:	31 f6                	xor    %esi,%esi
			/*
			 * ``... diouXx conversions ... if a precision is
			 * specified, the 0 flag will be ignored.''
			 *	-- ANSI X3J11
			 */
number:			if ((dprec = prec) >= 0)
 8049422:	85 ed                	test   %ebp,%ebp
 8049424:	78 08                	js     804942e <_vfprintf_r+0x7c2>
				flags &= ~ZEROPAD;
 8049426:	81 64 24 48 7f ff ff 	andl   $0xffffff7f,0x48(%esp)
 804942d:	ff 
			 * ``The result of converting a zero value with an
			 * explicit precision of zero is no characters.''
			 *	-- ANSI X3J11
			 */
			cp = buf + BUF;
			if (_uquad != 0 || prec != 0) {
 804942e:	85 c9                	test   %ecx,%ecx
 8049430:	0f 85 56 04 00 00    	jne    804988c <_vfprintf_r+0xc20>
 8049436:	85 ed                	test   %ebp,%ebp
 8049438:	0f 85 4e 04 00 00    	jne    804988c <_vfprintf_r+0xc20>
			*     -- ANSI X3J11
			*
			* To demonstrate this case, compile and run:
                        *    printf ("%#.0o",0);
			*/
                       else if (base == OCT && (flags & ALT))
 804943e:	84 c0                	test   %al,%al
 8049440:	0f 85 f6 08 00 00    	jne    8049d3c <_vfprintf_r+0x10d0>
 8049446:	f6 44 24 48 01       	testb  $0x1,0x48(%esp)
 804944b:	0f 84 eb 08 00 00    	je     8049d3c <_vfprintf_r+0x10d0>
                         *--cp = '0';
 8049451:	c6 84 24 ff 00 00 00 	movb   $0x30,0xff(%esp)
 8049458:	30 
 8049459:	8b 8c 24 94 00 00 00 	mov    0x94(%esp),%ecx
 8049460:	89 4c 24 50          	mov    %ecx,0x50(%esp)
 8049464:	8d 84 24 ff 00 00 00 	lea    0xff(%esp),%eax
 804946b:	89 44 24 60          	mov    %eax,0x60(%esp)
 804946f:	90                   	nop
 8049470:	8b 44 24 50          	mov    0x50(%esp),%eax
 8049474:	89 44 24 4c          	mov    %eax,0x4c(%esp)
 8049478:	39 e8                	cmp    %ebp,%eax
 804947a:	7d 04                	jge    8049480 <_vfprintf_r+0x814>
 804947c:	89 6c 24 4c          	mov    %ebp,0x4c(%esp)
		dprec = 0;
		width = 0;
		prec = -1;
		sign = '\0';
#ifdef FLOATING_POINT
		lead = 0;
 8049480:	89 6c 24 6c          	mov    %ebp,0x6c(%esp)
 8049484:	c7 44 24 78 00 00 00 	movl   $0x0,0x78(%esp)
 804948b:	00 
		 *
		 * Compute actual size, so we know how much to pad.
		 * size excludes decimal prec; realsz includes it.
		 */
		realsz = dprec > size ? dprec : size;
		if (sign)
 804948c:	89 f2                	mov    %esi,%edx
 804948e:	84 d2                	test   %dl,%dl
 8049490:	0f 84 b6 f9 ff ff    	je     8048e4c <_vfprintf_r+0x1e0>
			realsz++;
 8049496:	ff 44 24 4c          	incl   0x4c(%esp)
 804949a:	e9 ad f9 ff ff       	jmp    8048e4c <_vfprintf_r+0x1e0>
			/*
			 * ``If the space and + flags both appear, the space
			 * flag will be ignored.''
			 *	-- ANSI X3J11
			 */
			if (!sign)
 804949f:	84 c9                	test   %cl,%cl
 80494a1:	0f 85 46 f9 ff ff    	jne    8048ded <_vfprintf_r+0x181>
				sign = ' ';
 80494a7:	b1 20                	mov    $0x20,%cl
 80494a9:	e9 3f f9 ff ff       	jmp    8048ded <_vfprintf_r+0x181>
			goto rflag;
		case '#':
			flags |= ALT;
 80494ae:	83 4c 24 48 01       	orl    $0x1,0x48(%esp)
			goto rflag;
 80494b3:	e9 35 f9 ff ff       	jmp    8048ded <_vfprintf_r+0x181>
			 * ``A negative field width argument is taken as a
			 * - flag followed by a positive field width.''
			 *	-- ANSI X3J11
			 * They don't exclude field widths read from args.
			 */
			width = GET_ARG (n, ap, int);
 80494b8:	8b 84 24 6c 01 00 00 	mov    0x16c(%esp),%eax
 80494bf:	83 c0 04             	add    $0x4,%eax
 80494c2:	8b 94 24 6c 01 00 00 	mov    0x16c(%esp),%edx
 80494c9:	8b 12                	mov    (%edx),%edx
 80494cb:	89 54 24 5c          	mov    %edx,0x5c(%esp)
#ifndef _NO_POS_ARGS
			is_pos_arg = old_is_pos_arg;
#endif
			if (width >= 0)
 80494cf:	85 d2                	test   %edx,%edx
 80494d1:	0f 88 36 fd ff ff    	js     804920d <_vfprintf_r+0x5a1>
						fmt = old_fmt;
						goto rflag;
					}
				}
#endif /* !_NO_POS_ARGS */
				prec = GET_ARG (n, ap, int);
 80494d7:	89 84 24 6c 01 00 00 	mov    %eax,0x16c(%esp)
 80494de:	e9 0a f9 ff ff       	jmp    8048ded <_vfprintf_r+0x181>
			/* FALLTHROUGH */
		case '-':
			flags |= LADJUST;
			goto rflag;
		case '+':
			sign = '+';
 80494e3:	b1 2b                	mov    $0x2b,%cl
 80494e5:	e9 03 f9 ff ff       	jmp    8048ded <_vfprintf_r+0x181>
			if (*fmt == 'h') {
				fmt++;
				flags |= CHARINT;
			} else
#endif
				flags |= SHORTINT;
 80494ea:	83 4c 24 48 40       	orl    $0x40,0x48(%esp)
			goto rflag;
 80494ef:	e9 f9 f8 ff ff       	jmp    8048ded <_vfprintf_r+0x181>
 80494f4:	89 44 24 68          	mov    %eax,0x68(%esp)
 80494f8:	88 8c 24 ad 00 00 00 	mov    %cl,0xad(%esp)
			goto nosign;
		case 'X':
			xdigs = "0123456789ABCDEF";
			goto hex;
		case 'x':
			xdigs = "0123456789abcdef";
 80494ff:	c7 84 24 8c 00 00 00 	movl   $0x80516b5,0x8c(%esp)
 8049506:	b5 16 05 08 
hex:			_uquad = UARG ();
 804950a:	f6 44 24 48 10       	testb  $0x10,0x48(%esp)
 804950f:	0f 84 1b 02 00 00    	je     8049730 <_vfprintf_r+0xac4>
 8049515:	8b 84 24 6c 01 00 00 	mov    0x16c(%esp),%eax
 804951c:	8b 08                	mov    (%eax),%ecx
 804951e:	83 c0 04             	add    $0x4,%eax
 8049521:	89 84 24 6c 01 00 00 	mov    %eax,0x16c(%esp)
			base = HEX;
			/* leading 0x/X only if non-zero */
			if (flags & ALT && _uquad != 0) {
 8049528:	f6 44 24 48 01       	testb  $0x1,0x48(%esp)
 804952d:	0f 84 21 08 00 00    	je     8049d54 <_vfprintf_r+0x10e8>
 8049533:	85 c9                	test   %ecx,%ecx
 8049535:	0f 84 19 08 00 00    	je     8049d54 <_vfprintf_r+0x10e8>
				ox[0] = '0';
 804953b:	c6 84 24 ae 00 00 00 	movb   $0x30,0xae(%esp)
 8049542:	30 
				ox[1] = ch;
 8049543:	8a 44 24 68          	mov    0x68(%esp),%al
 8049547:	88 84 24 af 00 00 00 	mov    %al,0xaf(%esp)
				flags |= HEXPREFIX;
 804954e:	83 4c 24 48 02       	orl    $0x2,0x48(%esp)
			xdigs = "0123456789ABCDEF";
			goto hex;
		case 'x':
			xdigs = "0123456789abcdef";
hex:			_uquad = UARG ();
			base = HEX;
 8049553:	b0 02                	mov    $0x2,%al
 8049555:	e9 be fe ff ff       	jmp    8049418 <_vfprintf_r+0x7ac>
 804955a:	88 8c 24 ad 00 00 00 	mov    %cl,0xad(%esp)
#ifndef _NO_LONGLONG
			if (flags & QUADINT)
				*GET_ARG (N, ap, quad_ptr_t) = ret;
			else
#endif
			if (flags & LONGINT)
 8049561:	f6 44 24 48 10       	testb  $0x10,0x48(%esp)
 8049566:	0f 85 d3 02 00 00    	jne    804983f <_vfprintf_r+0xbd3>
				*GET_ARG (N, ap, long_ptr_t) = ret;
			else if (flags & SHORTINT)
 804956c:	f6 44 24 48 40       	testb  $0x40,0x48(%esp)
 8049571:	0f 84 c8 02 00 00    	je     804983f <_vfprintf_r+0xbd3>
				*GET_ARG (N, ap, short_ptr_t) = ret;
 8049577:	8b 8c 24 6c 01 00 00 	mov    0x16c(%esp),%ecx
 804957e:	8b 01                	mov    (%ecx),%eax
 8049580:	8b 54 24 54          	mov    0x54(%esp),%edx
 8049584:	66 89 10             	mov    %dx,(%eax)
 8049587:	83 c1 04             	add    $0x4,%ecx
 804958a:	89 8c 24 6c 01 00 00 	mov    %ecx,0x16c(%esp)
 8049591:	e9 dd f7 ff ff       	jmp    8048d73 <_vfprintf_r+0x107>
 8049596:	89 44 24 68          	mov    %eax,0x68(%esp)
 804959a:	88 8c 24 ad 00 00 00 	mov    %cl,0xad(%esp)
				size = 1;
			}
			sign = '\0';
			break;
		case 'D':  /* extension */
			flags |= LONGINT;
 80495a1:	83 4c 24 48 10       	orl    $0x10,0x48(%esp)
			/*FALLTHROUGH*/
		case 'd':
		case 'i':
			_uquad = SARG ();
 80495a6:	f6 44 24 48 10       	testb  $0x10,0x48(%esp)
 80495ab:	0f 85 f4 01 00 00    	jne    80497a5 <_vfprintf_r+0xb39>
 80495b1:	f6 44 24 48 40       	testb  $0x40,0x48(%esp)
 80495b6:	0f 84 e9 01 00 00    	je     80497a5 <_vfprintf_r+0xb39>
 80495bc:	8b 84 24 6c 01 00 00 	mov    0x16c(%esp),%eax
 80495c3:	0f bf 08             	movswl (%eax),%ecx
 80495c6:	83 c0 04             	add    $0x4,%eax
 80495c9:	89 84 24 6c 01 00 00 	mov    %eax,0x16c(%esp)
#ifndef _NO_LONGLONG
			if ((quad_t)_uquad < 0)
#else
			if ((long) _uquad < 0)
 80495d0:	85 c9                	test   %ecx,%ecx
 80495d2:	0f 88 f1 0c 00 00    	js     804a2c9 <_vfprintf_r+0x165d>
 80495d8:	0f b6 b4 24 ad 00 00 	movzbl 0xad(%esp),%esi
 80495df:	00 
			{

				_uquad = -_uquad;
				sign = '-';
			}
			base = DEC;
 80495e0:	b0 01                	mov    $0x1,%al
 80495e2:	e9 3b fe ff ff       	jmp    8049422 <_vfprintf_r+0x7b6>
 80495e7:	89 44 24 68          	mov    %eax,0x68(%esp)
 80495eb:	88 8c 24 ad 00 00 00 	mov    %cl,0xad(%esp)
		case 'f':
		case 'g':
		case 'G':
# ifdef _NO_LONGDBL
			if (flags & LONGDBL) {
				_fpvalue = (double) GET_ARG (N, ap, _LONG_DOUBLE);
 80495f2:	8b 8c 24 6c 01 00 00 	mov    0x16c(%esp),%ecx
		case 'E':
		case 'f':
		case 'g':
		case 'G':
# ifdef _NO_LONGDBL
			if (flags & LONGDBL) {
 80495f9:	f6 44 24 48 08       	testb  $0x8,0x48(%esp)
 80495fe:	0f 84 8a 0e 00 00    	je     804a48e <_vfprintf_r+0x1822>
				_fpvalue = (double) GET_ARG (N, ap, _LONG_DOUBLE);
 8049604:	db 29                	fldt   (%ecx)
 8049606:	dd 9c 24 80 00 00 00 	fstpl  0x80(%esp)
 804960d:	83 c1 0c             	add    $0xc,%ecx
 8049610:	89 8c 24 6c 01 00 00 	mov    %ecx,0x16c(%esp)

			   If the output is infinite or NaN, leading
			   zeros are not permitted.  Otherwise, scanf
			   could not read what printf wrote.
			 */
			if (isinf (_fpvalue)) {
 8049617:	dd 84 24 80 00 00 00 	fldl   0x80(%esp)
 804961e:	dd 1c 24             	fstpl  (%esp)
 8049621:	e8 de 5a 00 00       	call   804f104 <__fpclassifyd>
 8049626:	48                   	dec    %eax
 8049627:	0f 85 b2 0c 00 00    	jne    804a2df <_vfprintf_r+0x1673>
				if (_fpvalue < 0)
 804962d:	d9 ee                	fldz   
 804962f:	dd 84 24 80 00 00 00 	fldl   0x80(%esp)
 8049636:	d9 c9                	fxch   %st(1)
 8049638:	da e9                	fucompp 
 804963a:	df e0                	fnstsw %ax
 804963c:	f6 c4 45             	test   $0x45,%ah
 804963f:	0f 84 c9 12 00 00    	je     804a90e <_vfprintf_r+0x1ca2>
 8049645:	0f b6 b4 24 ad 00 00 	movzbl 0xad(%esp),%esi
 804964c:	00 
					sign = '-';
				if (ch <= 'G') /* 'A', 'E', 'F', or 'G' */
 804964d:	83 7c 24 68 47       	cmpl   $0x47,0x68(%esp)
 8049652:	0f 8e 35 10 00 00    	jle    804a68d <_vfprintf_r+0x1a21>
					cp = "INF";
				else
					cp = "inf";
 8049658:	c7 44 24 60 98 16 05 	movl   $0x8051698,0x60(%esp)
 804965f:	08 
				size = 3;
				flags &= ~ZEROPAD;
 8049660:	81 64 24 48 7f ff ff 	andl   $0xffffff7f,0x48(%esp)
 8049667:	ff 
				break;
 8049668:	c7 44 24 4c 03 00 00 	movl   $0x3,0x4c(%esp)
 804966f:	00 
					sign = '-';
				if (ch <= 'G') /* 'A', 'E', 'F', or 'G' */
					cp = "INF";
				else
					cp = "inf";
				size = 3;
 8049670:	c7 44 24 50 03 00 00 	movl   $0x3,0x50(%esp)
 8049677:	00 
#endif
		fmt_anchor = fmt;
		fmt++;		/* skip over '%' */

		flags = 0;
		dprec = 0;
 8049678:	c7 44 24 6c 00 00 00 	movl   $0x0,0x6c(%esp)
 804967f:	00 
		width = 0;
		prec = -1;
		sign = '\0';
#ifdef FLOATING_POINT
		lead = 0;
 8049680:	c7 44 24 78 00 00 00 	movl   $0x0,0x78(%esp)
 8049687:	00 
					cp = "INF";
				else
					cp = "inf";
				size = 3;
				flags &= ~ZEROPAD;
				break;
 8049688:	e9 ff fd ff ff       	jmp    804948c <_vfprintf_r+0x820>
#endif /* !_NO_POS_ARGS */
			width = n;
			goto reswitch;
#ifdef FLOATING_POINT
		case 'L':
			flags |= LONGDBL;
 804968d:	83 4c 24 48 08       	orl    $0x8,0x48(%esp)
			goto rflag;
 8049692:	e9 56 f7 ff ff       	jmp    8048ded <_vfprintf_r+0x181>
 8049697:	89 44 24 68          	mov    %eax,0x68(%esp)
#endif
			else
				*GET_ARG (N, ap, int_ptr_t) = ret;
			continue;	/* no output */
		case 'O': /* extension */
			flags |= LONGINT;
 804969b:	83 4c 24 48 10       	orl    $0x10,0x48(%esp)
			/*FALLTHROUGH*/
		case 'o':
			_uquad = UARG ();
 80496a0:	f6 44 24 48 10       	testb  $0x10,0x48(%esp)
 80496a5:	0f 84 07 08 00 00    	je     8049eb2 <_vfprintf_r+0x1246>
 80496ab:	8b 84 24 6c 01 00 00 	mov    0x16c(%esp),%eax
 80496b2:	8b 08                	mov    (%eax),%ecx
 80496b4:	83 c0 04             	add    $0x4,%eax
 80496b7:	89 84 24 6c 01 00 00 	mov    %eax,0x16c(%esp)
			base = OCT;
 80496be:	31 c0                	xor    %eax,%eax
 80496c0:	e9 53 fd ff ff       	jmp    8049418 <_vfprintf_r+0x7ac>
 80496c5:	89 44 24 68          	mov    %eax,0x68(%esp)
			} else
				size = strlen (cp);

			break;
		case 'U': /* extension */
			flags |= LONGINT;
 80496c9:	83 4c 24 48 10       	orl    $0x10,0x48(%esp)
			/*FALLTHROUGH*/
		case 'u':
			_uquad = UARG ();
 80496ce:	f6 44 24 48 10       	testb  $0x10,0x48(%esp)
 80496d3:	0f 85 83 06 00 00    	jne    8049d5c <_vfprintf_r+0x10f0>
 80496d9:	f6 44 24 48 40       	testb  $0x40,0x48(%esp)
 80496de:	0f 84 78 06 00 00    	je     8049d5c <_vfprintf_r+0x10f0>
		if (!isa)
			*p++ = '0';
		*p++ = to_char (exp);
	}
	return (p - p0);
}
 80496e4:	8b 8c 24 6c 01 00 00 	mov    0x16c(%esp),%ecx
 80496eb:	8b 09                	mov    (%ecx),%ecx
 80496ed:	89 4c 24 38          	mov    %ecx,0x38(%esp)
 80496f1:	81 e1 ff ff 00 00    	and    $0xffff,%ecx
			break;
		case 'U': /* extension */
			flags |= LONGINT;
			/*FALLTHROUGH*/
		case 'u':
			_uquad = UARG ();
 80496f7:	8b 84 24 6c 01 00 00 	mov    0x16c(%esp),%eax
 80496fe:	83 c0 04             	add    $0x4,%eax
 8049701:	89 84 24 6c 01 00 00 	mov    %eax,0x16c(%esp)
			base = DEC;
 8049708:	b0 01                	mov    $0x1,%al
 804970a:	e9 09 fd ff ff       	jmp    8049418 <_vfprintf_r+0x7ac>
 804970f:	89 44 24 68          	mov    %eax,0x68(%esp)
 8049713:	88 8c 24 ad 00 00 00 	mov    %cl,0xad(%esp)
			goto nosign;
		case 'X':
			xdigs = "0123456789ABCDEF";
 804971a:	c7 84 24 8c 00 00 00 	movl   $0x80516a4,0x8c(%esp)
 8049721:	a4 16 05 08 
			goto hex;
		case 'x':
			xdigs = "0123456789abcdef";
hex:			_uquad = UARG ();
 8049725:	f6 44 24 48 10       	testb  $0x10,0x48(%esp)
 804972a:	0f 85 e5 fd ff ff    	jne    8049515 <_vfprintf_r+0x8a9>
 8049730:	f6 44 24 48 40       	testb  $0x40,0x48(%esp)
 8049735:	0f 84 6b 0d 00 00    	je     804a4a6 <_vfprintf_r+0x183a>
		if (!isa)
			*p++ = '0';
		*p++ = to_char (exp);
	}
	return (p - p0);
}
 804973b:	8b 8c 24 6c 01 00 00 	mov    0x16c(%esp),%ecx
 8049742:	8b 09                	mov    (%ecx),%ecx
 8049744:	89 4c 24 34          	mov    %ecx,0x34(%esp)
 8049748:	81 e1 ff ff 00 00    	and    $0xffff,%ecx
		case 'X':
			xdigs = "0123456789ABCDEF";
			goto hex;
		case 'x':
			xdigs = "0123456789abcdef";
hex:			_uquad = UARG ();
 804974e:	8b 84 24 6c 01 00 00 	mov    0x16c(%esp),%eax
 8049755:	83 c0 04             	add    $0x4,%eax
 8049758:	89 84 24 6c 01 00 00 	mov    %eax,0x16c(%esp)
 804975f:	e9 c4 fd ff ff       	jmp    8049528 <_vfprintf_r+0x8bc>
 8049764:	89 44 24 68          	mov    %eax,0x68(%esp)
				}
			}
			else
#endif /* _MB_CAPABLE */
			{
				*cp = GET_ARG (N, ap, int);
 8049768:	8b 8c 24 6c 01 00 00 	mov    0x16c(%esp),%ecx
 804976f:	8b 01                	mov    (%ecx),%eax
 8049771:	88 84 24 d8 00 00 00 	mov    %al,0xd8(%esp)
				size = 1;
			}
			sign = '\0';
 8049778:	c6 84 24 ad 00 00 00 	movb   $0x0,0xad(%esp)
 804977f:	00 
				}
			}
			else
#endif /* _MB_CAPABLE */
			{
				*cp = GET_ARG (N, ap, int);
 8049780:	83 c1 04             	add    $0x4,%ecx
 8049783:	89 8c 24 6c 01 00 00 	mov    %ecx,0x16c(%esp)
 804978a:	e9 90 f6 ff ff       	jmp    8048e1f <_vfprintf_r+0x1b3>
 804978f:	89 44 24 68          	mov    %eax,0x68(%esp)
 8049793:	88 8c 24 ad 00 00 00 	mov    %cl,0xad(%esp)
		case 'D':  /* extension */
			flags |= LONGINT;
			/*FALLTHROUGH*/
		case 'd':
		case 'i':
			_uquad = SARG ();
 804979a:	f6 44 24 48 10       	testb  $0x10,0x48(%esp)
 804979f:	0f 84 0c fe ff ff    	je     80495b1 <_vfprintf_r+0x945>
 80497a5:	8b 94 24 6c 01 00 00 	mov    0x16c(%esp),%edx
 80497ac:	8b 0a                	mov    (%edx),%ecx
 80497ae:	83 c2 04             	add    $0x4,%edx
 80497b1:	89 94 24 6c 01 00 00 	mov    %edx,0x16c(%esp)
 80497b8:	e9 13 fe ff ff       	jmp    80495d0 <_vfprintf_r+0x964>
			/*
			 * ``Note that 0 is taken as a flag, not as the
			 * beginning of a field width.''
			 *	-- ANSI X3J11
			 */
			flags |= ZEROPAD;
 80497bd:	81 4c 24 48 80 00 00 	orl    $0x80,0x48(%esp)
 80497c4:	00 
			goto rflag;
 80497c5:	e9 23 f6 ff ff       	jmp    8048ded <_vfprintf_r+0x181>
		N = arg_index;
		is_pos_arg = 0;
#endif

rflag:		ch = *fmt++;
reswitch:	switch (ch) {
 80497ca:	31 f6                	xor    %esi,%esi
 80497cc:	8d 50 d0             	lea    -0x30(%eax),%edx
 80497cf:	90                   	nop
			goto rflag;
		case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
			n = 0;
			do {
				n = 10 * n + to_digit (ch);
 80497d0:	8d 04 b6             	lea    (%esi,%esi,4),%eax
 80497d3:	8d 34 42             	lea    (%edx,%eax,2),%esi
				ch = *fmt++;
 80497d6:	0f be 03             	movsbl (%ebx),%eax
 80497d9:	43                   	inc    %ebx
			} while (is_digit (ch));
 80497da:	8d 50 d0             	lea    -0x30(%eax),%edx
 80497dd:	83 fa 09             	cmp    $0x9,%edx
 80497e0:	76 ee                	jbe    80497d0 <_vfprintf_r+0xb64>
			goto rflag;
		case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
			n = 0;
			do {
				n = 10 * n + to_digit (ch);
 80497e2:	89 74 24 5c          	mov    %esi,0x5c(%esp)
 80497e6:	e9 06 f6 ff ff       	jmp    8048df1 <_vfprintf_r+0x185>
			goto rflag;
		case '+':
			sign = '+';
			goto rflag;
		case '.':
			if ((ch = *fmt++) == '*') {
 80497eb:	0f be 03             	movsbl (%ebx),%eax
 80497ee:	43                   	inc    %ebx
 80497ef:	83 f8 2a             	cmp    $0x2a,%eax
 80497f2:	0f 84 64 14 00 00    	je     804ac5c <_vfprintf_r+0x1ff0>
				if (prec < 0)
					prec = -1;
				goto rflag;
			}
			n = 0;
			while (is_digit (ch)) {
 80497f8:	8d 50 d0             	lea    -0x30(%eax),%edx
 80497fb:	31 ed                	xor    %ebp,%ebp
 80497fd:	83 fa 09             	cmp    $0x9,%edx
 8049800:	0f 87 eb f5 ff ff    	ja     8048df1 <_vfprintf_r+0x185>
 8049806:	66 90                	xchg   %ax,%ax
				n = 10 * n + to_digit (ch);
 8049808:	8d 44 ad 00          	lea    0x0(%ebp,%ebp,4),%eax
 804980c:	8d 2c 42             	lea    (%edx,%eax,2),%ebp
				ch = *fmt++;
 804980f:	0f be 03             	movsbl (%ebx),%eax
 8049812:	43                   	inc    %ebx
				if (prec < 0)
					prec = -1;
				goto rflag;
			}
			n = 0;
			while (is_digit (ch)) {
 8049813:	8d 50 d0             	lea    -0x30(%eax),%edx
 8049816:	83 fa 09             	cmp    $0x9,%edx
 8049819:	76 ed                	jbe    8049808 <_vfprintf_r+0xb9c>
 804981b:	85 ed                	test   %ebp,%ebp
 804981d:	0f 89 ce f5 ff ff    	jns    8048df1 <_vfprintf_r+0x185>
 8049823:	bd ff ff ff ff       	mov    $0xffffffff,%ebp
 8049828:	e9 c4 f5 ff ff       	jmp    8048df1 <_vfprintf_r+0x185>
 804982d:	89 44 24 68          	mov    %eax,0x68(%esp)
 8049831:	e9 6a fe ff ff       	jmp    80496a0 <_vfprintf_r+0xa34>
 8049836:	89 44 24 68          	mov    %eax,0x68(%esp)
 804983a:	e9 8f fe ff ff       	jmp    80496ce <_vfprintf_r+0xa62>
#ifdef _WANT_IO_C99_FORMATS
			else if (flags & CHARINT)
				*GET_ARG (N, ap, char_ptr_t) = ret;
#endif
			else
				*GET_ARG (N, ap, int_ptr_t) = ret;
 804983f:	8b 8c 24 6c 01 00 00 	mov    0x16c(%esp),%ecx
 8049846:	8b 01                	mov    (%ecx),%eax
 8049848:	8b 54 24 54          	mov    0x54(%esp),%edx
 804984c:	89 10                	mov    %edx,(%eax)
 804984e:	83 c1 04             	add    $0x4,%ecx
 8049851:	89 8c 24 6c 01 00 00 	mov    %ecx,0x16c(%esp)
 8049858:	e9 16 f5 ff ff       	jmp    8048d73 <_vfprintf_r+0x107>
 804985d:	89 6c 24 4c          	mov    %ebp,0x4c(%esp)
 8049861:	0f b6 b4 24 ad 00 00 	movzbl 0xad(%esp),%esi
 8049868:	00 
				 */
				char *p = memchr (cp, 0, prec);

				if (p != NULL) {
					size = p - cp;
					if (size > prec)
 8049869:	89 6c 24 50          	mov    %ebp,0x50(%esp)
			goto nosign;
		case 's':
#ifdef _WANT_IO_C99_FORMATS
		case 'S':
#endif
			cp = GET_ARG (N, ap, char_ptr_t);
 804986d:	89 94 24 6c 01 00 00 	mov    %edx,0x16c(%esp)
#endif
		fmt_anchor = fmt;
		fmt++;		/* skip over '%' */

		flags = 0;
		dprec = 0;
 8049874:	c7 44 24 6c 00 00 00 	movl   $0x0,0x6c(%esp)
 804987b:	00 
		width = 0;
		prec = -1;
		sign = '\0';
#ifdef FLOATING_POINT
		lead = 0;
 804987c:	c7 44 24 78 00 00 00 	movl   $0x0,0x78(%esp)
 8049883:	00 
 8049884:	e9 03 fc ff ff       	jmp    804948c <_vfprintf_r+0x820>
 8049889:	8d 76 00             	lea    0x0(%esi),%esi
				/*
				 * Unsigned mod is hard, and unsigned mod
				 * by a constant is easier than that by
				 * a variable; hence this switch.
				 */
				switch (base) {
 804988c:	3c 01                	cmp    $0x1,%al
 804988e:	0f 84 78 04 00 00    	je     8049d0c <_vfprintf_r+0x10a0>
 8049894:	3c 02                	cmp    $0x2,%al
			/*
			 * ``The result of converting a zero value with an
			 * explicit precision of zero is no characters.''
			 *	-- ANSI X3J11
			 */
			cp = buf + BUF;
 8049896:	8d 84 24 00 01 00 00 	lea    0x100(%esp),%eax
				/*
				 * Unsigned mod is hard, and unsigned mod
				 * by a constant is easier than that by
				 * a variable; hence this switch.
				 */
				switch (base) {
 804989d:	0f 84 32 04 00 00    	je     8049cd5 <_vfprintf_r+0x1069>
			/*
			 * ``The result of converting a zero value with an
			 * explicit precision of zero is no characters.''
			 *	-- ANSI X3J11
			 */
			cp = buf + BUF;
 80498a3:	89 c2                	mov    %eax,%edx
 80498a5:	8d 76 00             	lea    0x0(%esi),%esi
				 * a variable; hence this switch.
				 */
				switch (base) {
				case OCT:
					do {
						*--cp = to_char (_uquad & 7);
 80498a8:	4a                   	dec    %edx
 80498a9:	89 c8                	mov    %ecx,%eax
 80498ab:	83 e0 07             	and    $0x7,%eax
 80498ae:	83 c0 30             	add    $0x30,%eax
 80498b1:	88 02                	mov    %al,(%edx)
						_uquad >>= 3;
					} while (_uquad);
 80498b3:	c1 e9 03             	shr    $0x3,%ecx
 80498b6:	75 f0                	jne    80498a8 <_vfprintf_r+0xc3c>
 80498b8:	89 54 24 60          	mov    %edx,0x60(%esp)
					/* handle octal leading 0 */
					if (flags & ALT && *cp != '0')
 80498bc:	f6 44 24 48 01       	testb  $0x1,0x48(%esp)
 80498c1:	0f 85 1f 07 00 00    	jne    8049fe6 <_vfprintf_r+0x137a>
 80498c7:	8b 4c 24 44          	mov    0x44(%esp),%ecx
 80498cb:	29 d1                	sub    %edx,%ecx
 80498cd:	89 4c 24 50          	mov    %ecx,0x50(%esp)
 80498d1:	e9 9a fb ff ff       	jmp    8049470 <_vfprintf_r+0x804>
 80498d6:	66 90                	xchg   %ax,%ax
		/* the string or number proper */
#ifdef FLOATING_POINT
		if ((flags & FPT) == 0) {
			PRINT (cp, size);
		} else {	/* glue together f_p fragments */
			if (ch >= 'f') {	/* 'f' or 'g' */
 80498d8:	83 7c 24 68 65       	cmpl   $0x65,0x68(%esp)
 80498dd:	0f 8e f5 01 00 00    	jle    8049ad8 <_vfprintf_r+0xe6c>
				if (_fpvalue == 0) {
 80498e3:	d9 ee                	fldz   
 80498e5:	dd 84 24 80 00 00 00 	fldl   0x80(%esp)
 80498ec:	da e9                	fucompp 
 80498ee:	df e0                	fnstsw %ax
 80498f0:	80 e4 45             	and    $0x45,%ah
 80498f3:	80 f4 40             	xor    $0x40,%ah
 80498f6:	0f 85 7a 04 00 00    	jne    8049d76 <_vfprintf_r+0x110a>
					/* kludge for __dtoa irregularity */
					PRINT ("0", 1);
 80498fc:	c7 07 cd 16 05 08    	movl   $0x80516cd,(%edi)
 8049902:	c7 47 04 01 00 00 00 	movl   $0x1,0x4(%edi)
 8049909:	ff 84 24 d4 00 00 00 	incl   0xd4(%esp)
 8049910:	8b 94 24 d0 00 00 00 	mov    0xd0(%esp),%edx
 8049917:	42                   	inc    %edx
 8049918:	89 94 24 d0 00 00 00 	mov    %edx,0xd0(%esp)
 804991f:	83 fa 07             	cmp    $0x7,%edx
 8049922:	0f 8f e1 0b 00 00    	jg     804a509 <_vfprintf_r+0x189d>
 8049928:	83 c7 08             	add    $0x8,%edi
					if (expt < ndig || flags & ALT) {
 804992b:	8b 4c 24 70          	mov    0x70(%esp),%ecx
 804992f:	39 8c 24 b0 00 00 00 	cmp    %ecx,0xb0(%esp)
 8049936:	7c 0b                	jl     8049943 <_vfprintf_r+0xcd7>
 8049938:	f6 44 24 48 01       	testb  $0x1,0x48(%esp)
 804993d:	0f 84 83 f7 ff ff    	je     80490c6 <_vfprintf_r+0x45a>
						PRINT (decimal_point, decp_len);
 8049943:	8b 4c 24 74          	mov    0x74(%esp),%ecx
 8049947:	89 0f                	mov    %ecx,(%edi)
 8049949:	8b 44 24 7c          	mov    0x7c(%esp),%eax
 804994d:	89 47 04             	mov    %eax,0x4(%edi)
 8049950:	01 84 24 d4 00 00 00 	add    %eax,0xd4(%esp)
 8049957:	8b 94 24 d0 00 00 00 	mov    0xd0(%esp),%edx
 804995e:	42                   	inc    %edx
 804995f:	89 94 24 d0 00 00 00 	mov    %edx,0xd0(%esp)
 8049966:	83 fa 07             	cmp    $0x7,%edx
 8049969:	0f 8f 6b 0d 00 00    	jg     804a6da <_vfprintf_r+0x1a6e>
 804996f:	83 c7 08             	add    $0x8,%edi
						PAD (ndig - 1, zeroes);
 8049972:	8b 74 24 70          	mov    0x70(%esp),%esi
 8049976:	4e                   	dec    %esi
 8049977:	85 f6                	test   %esi,%esi
 8049979:	0f 8e 47 f7 ff ff    	jle    80490c6 <_vfprintf_r+0x45a>
 804997f:	8b 94 24 d0 00 00 00 	mov    0xd0(%esp),%edx
 8049986:	83 fe 10             	cmp    $0x10,%esi
 8049989:	0f 8e ee 05 00 00    	jle    8049f7d <_vfprintf_r+0x1311>
 804998f:	8d ac 24 cc 00 00 00 	lea    0xcc(%esp),%ebp
 8049996:	89 5c 24 50          	mov    %ebx,0x50(%esp)
 804999a:	89 f8                	mov    %edi,%eax
 804999c:	89 f7                	mov    %esi,%edi
 804999e:	8b 9c 24 60 01 00 00 	mov    0x160(%esp),%ebx
 80499a5:	8b b4 24 64 01 00 00 	mov    0x164(%esp),%esi
 80499ac:	eb 11                	jmp    80499bf <_vfprintf_r+0xd53>
 80499ae:	66 90                	xchg   %ax,%ax
 80499b0:	83 c0 08             	add    $0x8,%eax
 80499b3:	83 ef 10             	sub    $0x10,%edi
 80499b6:	83 ff 10             	cmp    $0x10,%edi
 80499b9:	0f 8e b6 05 00 00    	jle    8049f75 <_vfprintf_r+0x1309>
 80499bf:	c7 00 44 18 05 08    	movl   $0x8051844,(%eax)
 80499c5:	c7 40 04 10 00 00 00 	movl   $0x10,0x4(%eax)
 80499cc:	83 84 24 d4 00 00 00 	addl   $0x10,0xd4(%esp)
 80499d3:	10 
 80499d4:	42                   	inc    %edx
 80499d5:	89 94 24 d0 00 00 00 	mov    %edx,0xd0(%esp)
 80499dc:	83 fa 07             	cmp    $0x7,%edx
 80499df:	7e cf                	jle    80499b0 <_vfprintf_r+0xd44>
 80499e1:	89 6c 24 08          	mov    %ebp,0x8(%esp)
 80499e5:	89 74 24 04          	mov    %esi,0x4(%esp)
 80499e9:	89 1c 24             	mov    %ebx,(%esp)
 80499ec:	e8 fb 58 00 00       	call   804f2ec <__sprint_r>
 80499f1:	85 c0                	test   %eax,%eax
 80499f3:	0f 85 d7 f8 ff ff    	jne    80492d0 <_vfprintf_r+0x664>
 80499f9:	8d 84 24 00 01 00 00 	lea    0x100(%esp),%eax
 8049a00:	8b 94 24 d0 00 00 00 	mov    0xd0(%esp),%edx
 8049a07:	eb aa                	jmp    80499b3 <_vfprintf_r+0xd47>
 8049a09:	8d 76 00             	lea    0x0(%esi),%esi
		if (flags & HEXPREFIX)
			PRINT (ox, 2);

		/* right-adjusting zero padding */
		if ((flags & (LADJUST|ZEROPAD)) == ZEROPAD)
			PAD (width - realsz, zeroes);
 8049a0c:	8b 6c 24 5c          	mov    0x5c(%esp),%ebp
 8049a10:	2b 6c 24 4c          	sub    0x4c(%esp),%ebp
 8049a14:	85 ed                	test   %ebp,%ebp
 8049a16:	0f 8e a9 f5 ff ff    	jle    8048fc5 <_vfprintf_r+0x359>
 8049a1c:	8b 94 24 d0 00 00 00 	mov    0xd0(%esp),%edx
 8049a23:	83 fd 10             	cmp    $0x10,%ebp
 8049a26:	0f 8e 81 00 00 00    	jle    8049aad <_vfprintf_r+0xe41>
 8049a2c:	8d b4 24 cc 00 00 00 	lea    0xcc(%esp),%esi
 8049a33:	89 5c 24 58          	mov    %ebx,0x58(%esp)
 8049a37:	89 f8                	mov    %edi,%eax
 8049a39:	8b 9c 24 60 01 00 00 	mov    0x160(%esp),%ebx
 8049a40:	8b bc 24 64 01 00 00 	mov    0x164(%esp),%edi
 8049a47:	eb 0e                	jmp    8049a57 <_vfprintf_r+0xdeb>
 8049a49:	8d 76 00             	lea    0x0(%esi),%esi
 8049a4c:	83 c0 08             	add    $0x8,%eax
 8049a4f:	83 ed 10             	sub    $0x10,%ebp
 8049a52:	83 fd 10             	cmp    $0x10,%ebp
 8049a55:	7e 50                	jle    8049aa7 <_vfprintf_r+0xe3b>
 8049a57:	c7 00 44 18 05 08    	movl   $0x8051844,(%eax)
 8049a5d:	c7 40 04 10 00 00 00 	movl   $0x10,0x4(%eax)
 8049a64:	83 84 24 d4 00 00 00 	addl   $0x10,0xd4(%esp)
 8049a6b:	10 
 8049a6c:	42                   	inc    %edx
 8049a6d:	89 94 24 d0 00 00 00 	mov    %edx,0xd0(%esp)
 8049a74:	83 fa 07             	cmp    $0x7,%edx
 8049a77:	7e d3                	jle    8049a4c <_vfprintf_r+0xde0>
 8049a79:	89 74 24 08          	mov    %esi,0x8(%esp)
 8049a7d:	89 7c 24 04          	mov    %edi,0x4(%esp)
 8049a81:	89 1c 24             	mov    %ebx,(%esp)
 8049a84:	e8 63 58 00 00       	call   804f2ec <__sprint_r>
 8049a89:	85 c0                	test   %eax,%eax
 8049a8b:	0f 85 3f f8 ff ff    	jne    80492d0 <_vfprintf_r+0x664>
 8049a91:	8d 84 24 00 01 00 00 	lea    0x100(%esp),%eax
 8049a98:	8b 94 24 d0 00 00 00 	mov    0xd0(%esp),%edx
 8049a9f:	83 ed 10             	sub    $0x10,%ebp
 8049aa2:	83 fd 10             	cmp    $0x10,%ebp
 8049aa5:	7f b0                	jg     8049a57 <_vfprintf_r+0xdeb>
 8049aa7:	8b 5c 24 58          	mov    0x58(%esp),%ebx
 8049aab:	89 c7                	mov    %eax,%edi
 8049aad:	c7 07 44 18 05 08    	movl   $0x8051844,(%edi)
 8049ab3:	89 6f 04             	mov    %ebp,0x4(%edi)
 8049ab6:	01 ac 24 d4 00 00 00 	add    %ebp,0xd4(%esp)
 8049abd:	42                   	inc    %edx
 8049abe:	89 94 24 d0 00 00 00 	mov    %edx,0xd0(%esp)
 8049ac5:	83 fa 07             	cmp    $0x7,%edx
 8049ac8:	0f 8f c2 07 00 00    	jg     804a290 <_vfprintf_r+0x1624>
 8049ace:	83 c7 08             	add    $0x8,%edi
 8049ad1:	e9 ef f4 ff ff       	jmp    8048fc5 <_vfprintf_r+0x359>
 8049ad6:	66 90                	xchg   %ax,%ax
					PRINTANDPAD (cp, convbuf + ndig,
						     ndig - expt, zeroes);
				}
			} else {	/* 'a', 'A', 'e', or 'E' */
				if (ndig > 1 || flags & ALT) {
					PRINT (cp, 1);
 8049ad8:	8b 44 24 60          	mov    0x60(%esp),%eax
					    PRINT (decimal_point, decp_len);
					PRINTANDPAD (cp, convbuf + ndig,
						     ndig - expt, zeroes);
				}
			} else {	/* 'a', 'A', 'e', or 'E' */
				if (ndig > 1 || flags & ALT) {
 8049adc:	83 7c 24 70 01       	cmpl   $0x1,0x70(%esp)
 8049ae1:	0f 8e 8a 06 00 00    	jle    804a171 <_vfprintf_r+0x1505>
					PRINT (cp, 1);
 8049ae7:	89 07                	mov    %eax,(%edi)
 8049ae9:	c7 47 04 01 00 00 00 	movl   $0x1,0x4(%edi)
 8049af0:	ff 84 24 d4 00 00 00 	incl   0xd4(%esp)
 8049af7:	8b 94 24 d0 00 00 00 	mov    0xd0(%esp),%edx
 8049afe:	42                   	inc    %edx
 8049aff:	89 94 24 d0 00 00 00 	mov    %edx,0xd0(%esp)
 8049b06:	83 fa 07             	cmp    $0x7,%edx
 8049b09:	0f 8f d5 06 00 00    	jg     804a1e4 <_vfprintf_r+0x1578>
 8049b0f:	83 c7 08             	add    $0x8,%edi
					cp++;
					PRINT (decimal_point, decp_len);
 8049b12:	8b 4c 24 74          	mov    0x74(%esp),%ecx
 8049b16:	89 0f                	mov    %ecx,(%edi)
 8049b18:	8b 44 24 7c          	mov    0x7c(%esp),%eax
 8049b1c:	89 47 04             	mov    %eax,0x4(%edi)
 8049b1f:	01 84 24 d4 00 00 00 	add    %eax,0xd4(%esp)
 8049b26:	42                   	inc    %edx
 8049b27:	89 94 24 d0 00 00 00 	mov    %edx,0xd0(%esp)
 8049b2e:	83 fa 07             	cmp    $0x7,%edx
 8049b31:	0f 8f 19 07 00 00    	jg     804a250 <_vfprintf_r+0x15e4>
 8049b37:	83 c7 08             	add    $0x8,%edi
					if (_fpvalue) {
 8049b3a:	d9 ee                	fldz   
 8049b3c:	dd 84 24 80 00 00 00 	fldl   0x80(%esp)
 8049b43:	da e9                	fucompp 
 8049b45:	df e0                	fnstsw %ax
 8049b47:	80 e4 45             	and    $0x45,%ah
 8049b4a:	80 fc 40             	cmp    $0x40,%ah
 8049b4d:	0f 84 95 03 00 00    	je     8049ee8 <_vfprintf_r+0x127c>
						     ndig - expt, zeroes);
				}
			} else {	/* 'a', 'A', 'e', or 'E' */
				if (ndig > 1 || flags & ALT) {
					PRINT (cp, 1);
					cp++;
 8049b53:	8b 44 24 60          	mov    0x60(%esp),%eax
 8049b57:	40                   	inc    %eax
 8049b58:	89 07                	mov    %eax,(%edi)
					PRINT (decimal_point, decp_len);
					if (_fpvalue) {
						PRINT (cp, ndig - 1);
 8049b5a:	8b 44 24 70          	mov    0x70(%esp),%eax
 8049b5e:	48                   	dec    %eax
 8049b5f:	89 47 04             	mov    %eax,0x4(%edi)
 8049b62:	8b 84 24 d4 00 00 00 	mov    0xd4(%esp),%eax
 8049b69:	8b 4c 24 70          	mov    0x70(%esp),%ecx
 8049b6d:	8d 44 01 ff          	lea    -0x1(%ecx,%eax,1),%eax
 8049b71:	89 84 24 d4 00 00 00 	mov    %eax,0xd4(%esp)
 8049b78:	42                   	inc    %edx
 8049b79:	89 94 24 d0 00 00 00 	mov    %edx,0xd0(%esp)
 8049b80:	83 fa 07             	cmp    $0x7,%edx
 8049b83:	0f 8f 1b 06 00 00    	jg     804a1a4 <_vfprintf_r+0x1538>
					} else	/* 0.[0..] */
						/* __dtoa irregularity */
						PAD (ndig - 1, zeroes);
				} else	/* XeYYY */
					PRINT (cp, 1);
 8049b89:	83 c7 08             	add    $0x8,%edi
				PRINT (expstr, expsize);
 8049b8c:	8d 84 24 be 00 00 00 	lea    0xbe(%esp),%eax
 8049b93:	89 07                	mov    %eax,(%edi)
 8049b95:	8b 8c 24 88 00 00 00 	mov    0x88(%esp),%ecx
 8049b9c:	89 4f 04             	mov    %ecx,0x4(%edi)
 8049b9f:	01 8c 24 d4 00 00 00 	add    %ecx,0xd4(%esp)
 8049ba6:	42                   	inc    %edx
 8049ba7:	89 94 24 d0 00 00 00 	mov    %edx,0xd0(%esp)
 8049bae:	83 fa 07             	cmp    $0x7,%edx
 8049bb1:	0f 8e 0c f5 ff ff    	jle    80490c3 <_vfprintf_r+0x457>
		PAD (dprec - size, zeroes);

		/* the string or number proper */
#ifdef FLOATING_POINT
		if ((flags & FPT) == 0) {
			PRINT (cp, size);
 8049bb7:	8d 84 24 cc 00 00 00 	lea    0xcc(%esp),%eax
 8049bbe:	89 44 24 08          	mov    %eax,0x8(%esp)
 8049bc2:	8b 94 24 64 01 00 00 	mov    0x164(%esp),%edx
 8049bc9:	89 54 24 04          	mov    %edx,0x4(%esp)
 8049bcd:	8b 84 24 60 01 00 00 	mov    0x160(%esp),%eax
 8049bd4:	89 04 24             	mov    %eax,(%esp)
 8049bd7:	e8 10 57 00 00       	call   804f2ec <__sprint_r>
 8049bdc:	85 c0                	test   %eax,%eax
 8049bde:	0f 85 ec f6 ff ff    	jne    80492d0 <_vfprintf_r+0x664>
 8049be4:	8d bc 24 00 01 00 00 	lea    0x100(%esp),%edi
 8049beb:	e9 d6 f4 ff ff       	jmp    80490c6 <_vfprintf_r+0x45a>
			PAD (width - realsz, blanks);

		/* finally, adjust ret */
		ret += width > realsz ? width : realsz;

		FLUSH ();	/* copy out the I/O vectors */
 8049bf0:	8d 84 24 cc 00 00 00 	lea    0xcc(%esp),%eax
 8049bf7:	89 44 24 08          	mov    %eax,0x8(%esp)
 8049bfb:	8b 94 24 64 01 00 00 	mov    0x164(%esp),%edx
 8049c02:	89 54 24 04          	mov    %edx,0x4(%esp)
 8049c06:	8b 84 24 60 01 00 00 	mov    0x160(%esp),%eax
 8049c0d:	89 04 24             	mov    %eax,(%esp)
 8049c10:	e8 d7 56 00 00       	call   804f2ec <__sprint_r>
 8049c15:	85 c0                	test   %eax,%eax
 8049c17:	0f 84 c8 f5 ff ff    	je     80491e5 <_vfprintf_r+0x579>
 8049c1d:	e9 ae f6 ff ff       	jmp    80492d0 <_vfprintf_r+0x664>
 8049c22:	66 90                	xchg   %ax,%ax
		/* right-adjusting zero padding */
		if ((flags & (LADJUST|ZEROPAD)) == ZEROPAD)
			PAD (width - realsz, zeroes);

		/* leading zeroes from decimal precision */
		PAD (dprec - size, zeroes);
 8049c24:	8d 84 24 cc 00 00 00 	lea    0xcc(%esp),%eax
 8049c2b:	89 44 24 08          	mov    %eax,0x8(%esp)
 8049c2f:	8b 94 24 64 01 00 00 	mov    0x164(%esp),%edx
 8049c36:	89 54 24 04          	mov    %edx,0x4(%esp)
 8049c3a:	8b 84 24 60 01 00 00 	mov    0x160(%esp),%eax
 8049c41:	89 04 24             	mov    %eax,(%esp)
 8049c44:	e8 a3 56 00 00       	call   804f2ec <__sprint_r>
 8049c49:	85 c0                	test   %eax,%eax
 8049c4b:	0f 85 7f f6 ff ff    	jne    80492d0 <_vfprintf_r+0x664>
 8049c51:	8d bc 24 00 01 00 00 	lea    0x100(%esp),%edi
 8049c58:	e9 2c f4 ff ff       	jmp    8049089 <_vfprintf_r+0x41d>
 8049c5d:	8d 76 00             	lea    0x0(%esi),%esi
		if ((flags & (LADJUST|ZEROPAD)) == 0)
			PAD (width - realsz, blanks);

		/* prefix */
		if (sign)
			PRINT (&sign, 1);
 8049c60:	8d 84 24 cc 00 00 00 	lea    0xcc(%esp),%eax
 8049c67:	89 44 24 08          	mov    %eax,0x8(%esp)
 8049c6b:	8b 94 24 64 01 00 00 	mov    0x164(%esp),%edx
 8049c72:	89 54 24 04          	mov    %edx,0x4(%esp)
 8049c76:	8b 84 24 60 01 00 00 	mov    0x160(%esp),%eax
 8049c7d:	89 04 24             	mov    %eax,(%esp)
 8049c80:	e8 67 56 00 00       	call   804f2ec <__sprint_r>
 8049c85:	85 c0                	test   %eax,%eax
 8049c87:	0f 85 43 f6 ff ff    	jne    80492d0 <_vfprintf_r+0x664>
 8049c8d:	8d bc 24 00 01 00 00 	lea    0x100(%esp),%edi
 8049c94:	e9 e3 f2 ff ff       	jmp    8048f7c <_vfprintf_r+0x310>
 8049c99:	8d 76 00             	lea    0x0(%esi),%esi
		if (flags & HEXPREFIX)
			PRINT (ox, 2);
 8049c9c:	8d 84 24 cc 00 00 00 	lea    0xcc(%esp),%eax
 8049ca3:	89 44 24 08          	mov    %eax,0x8(%esp)
 8049ca7:	8b 94 24 64 01 00 00 	mov    0x164(%esp),%edx
 8049cae:	89 54 24 04          	mov    %edx,0x4(%esp)
 8049cb2:	8b 84 24 60 01 00 00 	mov    0x160(%esp),%eax
 8049cb9:	89 04 24             	mov    %eax,(%esp)
 8049cbc:	e8 2b 56 00 00       	call   804f2ec <__sprint_r>
 8049cc1:	85 c0                	test   %eax,%eax
 8049cc3:	0f 85 07 f6 ff ff    	jne    80492d0 <_vfprintf_r+0x664>
 8049cc9:	8d bc 24 00 01 00 00 	lea    0x100(%esp),%edi
 8049cd0:	e9 e2 f2 ff ff       	jmp    8048fb7 <_vfprintf_r+0x34b>
			/*
			 * ``The result of converting a zero value with an
			 * explicit precision of zero is no characters.''
			 *	-- ANSI X3J11
			 */
			cp = buf + BUF;
 8049cd5:	89 44 24 60          	mov    %eax,0x60(%esp)
 8049cd9:	89 5c 24 50          	mov    %ebx,0x50(%esp)
 8049cdd:	8b 9c 24 8c 00 00 00 	mov    0x8c(%esp),%ebx
					} while (_uquad != 0);
					break;

				case HEX:
					do {
						*--cp = xdigs[_uquad & 15];
 8049ce4:	48                   	dec    %eax
 8049ce5:	89 ca                	mov    %ecx,%edx
 8049ce7:	83 e2 0f             	and    $0xf,%edx
 8049cea:	8a 14 13             	mov    (%ebx,%edx,1),%dl
 8049ced:	88 10                	mov    %dl,(%eax)
						_uquad >>= 4;
					} while (_uquad);
 8049cef:	c1 e9 04             	shr    $0x4,%ecx
 8049cf2:	75 f0                	jne    8049ce4 <_vfprintf_r+0x1078>
 8049cf4:	8b 5c 24 50          	mov    0x50(%esp),%ebx
 8049cf8:	89 44 24 60          	mov    %eax,0x60(%esp)
 8049cfc:	8b 54 24 44          	mov    0x44(%esp),%edx
 8049d00:	29 c2                	sub    %eax,%edx
 8049d02:	89 54 24 50          	mov    %edx,0x50(%esp)
 8049d06:	e9 65 f7 ff ff       	jmp    8049470 <_vfprintf_r+0x804>
 8049d0b:	90                   	nop
						*--cp = '0';
					break;

				case DEC:
					/* many numbers are 1 digit */
					if (_uquad < 10) {
 8049d0c:	83 f9 09             	cmp    $0x9,%ecx
 8049d0f:	0f 87 8e 02 00 00    	ja     8049fa3 <_vfprintf_r+0x1337>
						*--cp = to_char(_uquad);
 8049d15:	83 c1 30             	add    $0x30,%ecx
 8049d18:	88 8c 24 ff 00 00 00 	mov    %cl,0xff(%esp)
 8049d1f:	8b 8c 24 94 00 00 00 	mov    0x94(%esp),%ecx
 8049d26:	89 4c 24 50          	mov    %ecx,0x50(%esp)
 8049d2a:	8d 84 24 ff 00 00 00 	lea    0xff(%esp),%eax
 8049d31:	89 44 24 60          	mov    %eax,0x60(%esp)
						break;
 8049d35:	e9 36 f7 ff ff       	jmp    8049470 <_vfprintf_r+0x804>
 8049d3a:	66 90                	xchg   %ax,%ax
			*     -- ANSI X3J11
			*
			* To demonstrate this case, compile and run:
                        *    printf ("%#.0o",0);
			*/
                       else if (base == OCT && (flags & ALT))
 8049d3c:	c7 44 24 50 00 00 00 	movl   $0x0,0x50(%esp)
 8049d43:	00 
			/*
			 * ``The result of converting a zero value with an
			 * explicit precision of zero is no characters.''
			 *	-- ANSI X3J11
			 */
			cp = buf + BUF;
 8049d44:	8d 84 24 00 01 00 00 	lea    0x100(%esp),%eax
 8049d4b:	89 44 24 60          	mov    %eax,0x60(%esp)
 8049d4f:	e9 1c f7 ff ff       	jmp    8049470 <_vfprintf_r+0x804>
			xdigs = "0123456789ABCDEF";
			goto hex;
		case 'x':
			xdigs = "0123456789abcdef";
hex:			_uquad = UARG ();
			base = HEX;
 8049d54:	b0 02                	mov    $0x2,%al
 8049d56:	e9 bd f6 ff ff       	jmp    8049418 <_vfprintf_r+0x7ac>
 8049d5b:	90                   	nop
			break;
		case 'U': /* extension */
			flags |= LONGINT;
			/*FALLTHROUGH*/
		case 'u':
			_uquad = UARG ();
 8049d5c:	8b 94 24 6c 01 00 00 	mov    0x16c(%esp),%edx
 8049d63:	8b 0a                	mov    (%edx),%ecx
 8049d65:	83 c2 04             	add    $0x4,%edx
 8049d68:	89 94 24 6c 01 00 00 	mov    %edx,0x16c(%esp)
			base = DEC;
 8049d6f:	b0 01                	mov    $0x1,%al
 8049d71:	e9 a2 f6 ff ff       	jmp    8049418 <_vfprintf_r+0x7ac>
					PRINT ("0", 1);
					if (expt < ndig || flags & ALT) {
						PRINT (decimal_point, decp_len);
						PAD (ndig - 1, zeroes);
					}
				} else if (expt <= 0) {
 8049d76:	8b b4 24 b0 00 00 00 	mov    0xb0(%esp),%esi
 8049d7d:	85 f6                	test   %esi,%esi
 8049d7f:	0f 8e bd 07 00 00    	jle    804a542 <_vfprintf_r+0x18d6>
						PAD (-expt, zeroes);
						PRINT (cp, ndig);
					}
				} else {
					char *convbuf = cp;
					PRINTANDPAD(cp, convbuf + ndig,
 8049d85:	8b 54 24 60          	mov    0x60(%esp),%edx
 8049d89:	03 54 24 70          	add    0x70(%esp),%edx
 8049d8d:	89 54 24 50          	mov    %edx,0x50(%esp)
 8049d91:	89 d6                	mov    %edx,%esi
 8049d93:	2b 74 24 60          	sub    0x60(%esp),%esi
 8049d97:	3b 74 24 78          	cmp    0x78(%esp),%esi
 8049d9b:	7e 04                	jle    8049da1 <_vfprintf_r+0x1135>
 8049d9d:	8b 74 24 78          	mov    0x78(%esp),%esi
 8049da1:	85 f6                	test   %esi,%esi
 8049da3:	7e 2b                	jle    8049dd0 <_vfprintf_r+0x1164>
 8049da5:	8b 44 24 60          	mov    0x60(%esp),%eax
 8049da9:	89 07                	mov    %eax,(%edi)
 8049dab:	89 77 04             	mov    %esi,0x4(%edi)
 8049dae:	01 b4 24 d4 00 00 00 	add    %esi,0xd4(%esp)
 8049db5:	8b 94 24 d0 00 00 00 	mov    0xd0(%esp),%edx
 8049dbc:	42                   	inc    %edx
 8049dbd:	89 94 24 d0 00 00 00 	mov    %edx,0xd0(%esp)
 8049dc4:	83 fa 07             	cmp    $0x7,%edx
 8049dc7:	0f 8f d2 09 00 00    	jg     804a79f <_vfprintf_r+0x1b33>
 8049dcd:	83 c7 08             	add    $0x8,%edi
 8049dd0:	85 f6                	test   %esi,%esi
 8049dd2:	0f 88 00 0a 00 00    	js     804a7d8 <_vfprintf_r+0x1b6c>
 8049dd8:	8b 54 24 78          	mov    0x78(%esp),%edx
 8049ddc:	29 f2                	sub    %esi,%edx
 8049dde:	89 d6                	mov    %edx,%esi
 8049de0:	85 d2                	test   %edx,%edx
 8049de2:	0f 8e 53 02 00 00    	jle    804a03b <_vfprintf_r+0x13cf>
 8049de8:	8b 94 24 d0 00 00 00 	mov    0xd0(%esp),%edx
 8049def:	83 fe 10             	cmp    $0x10,%esi
 8049df2:	0f 8e e8 06 00 00    	jle    804a4e0 <_vfprintf_r+0x1874>
 8049df8:	8d ac 24 cc 00 00 00 	lea    0xcc(%esp),%ebp
 8049dff:	89 5c 24 58          	mov    %ebx,0x58(%esp)
 8049e03:	89 f8                	mov    %edi,%eax
 8049e05:	89 f7                	mov    %esi,%edi
 8049e07:	8b 9c 24 60 01 00 00 	mov    0x160(%esp),%ebx
 8049e0e:	8b b4 24 64 01 00 00 	mov    0x164(%esp),%esi
 8049e15:	eb 10                	jmp    8049e27 <_vfprintf_r+0x11bb>
 8049e17:	90                   	nop
 8049e18:	83 c0 08             	add    $0x8,%eax
 8049e1b:	83 ef 10             	sub    $0x10,%edi
 8049e1e:	83 ff 10             	cmp    $0x10,%edi
 8049e21:	0f 8e b1 06 00 00    	jle    804a4d8 <_vfprintf_r+0x186c>
 8049e27:	c7 00 44 18 05 08    	movl   $0x8051844,(%eax)
 8049e2d:	c7 40 04 10 00 00 00 	movl   $0x10,0x4(%eax)
 8049e34:	83 84 24 d4 00 00 00 	addl   $0x10,0xd4(%esp)
 8049e3b:	10 
 8049e3c:	42                   	inc    %edx
 8049e3d:	89 94 24 d0 00 00 00 	mov    %edx,0xd0(%esp)
 8049e44:	83 fa 07             	cmp    $0x7,%edx
 8049e47:	7e cf                	jle    8049e18 <_vfprintf_r+0x11ac>
 8049e49:	89 6c 24 08          	mov    %ebp,0x8(%esp)
 8049e4d:	89 74 24 04          	mov    %esi,0x4(%esp)
 8049e51:	89 1c 24             	mov    %ebx,(%esp)
 8049e54:	e8 93 54 00 00       	call   804f2ec <__sprint_r>
 8049e59:	85 c0                	test   %eax,%eax
 8049e5b:	0f 85 6f f4 ff ff    	jne    80492d0 <_vfprintf_r+0x664>
 8049e61:	8d 84 24 00 01 00 00 	lea    0x100(%esp),%eax
 8049e68:	8b 94 24 d0 00 00 00 	mov    0xd0(%esp),%edx
 8049e6f:	eb aa                	jmp    8049e1b <_vfprintf_r+0x11af>
		if (flags & HEXPREFIX)
			realsz+= 2;

		/* right-adjusting blank padding */
		if ((flags & (LADJUST|ZEROPAD)) == 0)
			PAD (width - realsz, blanks);
 8049e71:	8d 84 24 cc 00 00 00 	lea    0xcc(%esp),%eax
 8049e78:	89 44 24 08          	mov    %eax,0x8(%esp)
 8049e7c:	8b 94 24 64 01 00 00 	mov    0x164(%esp),%edx
 8049e83:	89 54 24 04          	mov    %edx,0x4(%esp)
 8049e87:	8b 84 24 60 01 00 00 	mov    0x160(%esp),%eax
 8049e8e:	89 04 24             	mov    %eax,(%esp)
 8049e91:	e8 56 54 00 00       	call   804f2ec <__sprint_r>
 8049e96:	85 c0                	test   %eax,%eax
 8049e98:	0f 85 32 f4 ff ff    	jne    80492d0 <_vfprintf_r+0x664>
 8049e9e:	0f b6 b4 24 ad 00 00 	movzbl 0xad(%esp),%esi
 8049ea5:	00 
 8049ea6:	8d bc 24 00 01 00 00 	lea    0x100(%esp),%edi
 8049ead:	e9 92 f0 ff ff       	jmp    8048f44 <_vfprintf_r+0x2d8>
			continue;	/* no output */
		case 'O': /* extension */
			flags |= LONGINT;
			/*FALLTHROUGH*/
		case 'o':
			_uquad = UARG ();
 8049eb2:	f6 44 24 48 40       	testb  $0x40,0x48(%esp)
 8049eb7:	0f 84 01 06 00 00    	je     804a4be <_vfprintf_r+0x1852>
		if (!isa)
			*p++ = '0';
		*p++ = to_char (exp);
	}
	return (p - p0);
}
 8049ebd:	8b 8c 24 6c 01 00 00 	mov    0x16c(%esp),%ecx
 8049ec4:	8b 09                	mov    (%ecx),%ecx
 8049ec6:	89 4c 24 3c          	mov    %ecx,0x3c(%esp)
 8049eca:	81 e1 ff ff 00 00    	and    $0xffff,%ecx
			continue;	/* no output */
		case 'O': /* extension */
			flags |= LONGINT;
			/*FALLTHROUGH*/
		case 'o':
			_uquad = UARG ();
 8049ed0:	8b 84 24 6c 01 00 00 	mov    0x16c(%esp),%eax
 8049ed7:	83 c0 04             	add    $0x4,%eax
 8049eda:	89 84 24 6c 01 00 00 	mov    %eax,0x16c(%esp)
			base = OCT;
 8049ee1:	31 c0                	xor    %eax,%eax
 8049ee3:	e9 30 f5 ff ff       	jmp    8049418 <_vfprintf_r+0x7ac>
					PRINT (decimal_point, decp_len);
					if (_fpvalue) {
						PRINT (cp, ndig - 1);
					} else	/* 0.[0..] */
						/* __dtoa irregularity */
						PAD (ndig - 1, zeroes);
 8049ee8:	8b 6c 24 70          	mov    0x70(%esp),%ebp
 8049eec:	4d                   	dec    %ebp
 8049eed:	85 ed                	test   %ebp,%ebp
 8049eef:	0f 8e 97 fc ff ff    	jle    8049b8c <_vfprintf_r+0xf20>
 8049ef5:	83 fd 10             	cmp    $0x10,%ebp
 8049ef8:	0f 8e 2c 03 00 00    	jle    804a22a <_vfprintf_r+0x15be>
 8049efe:	8d b4 24 cc 00 00 00 	lea    0xcc(%esp),%esi
 8049f05:	89 5c 24 50          	mov    %ebx,0x50(%esp)
 8049f09:	89 f8                	mov    %edi,%eax
 8049f0b:	8b 9c 24 60 01 00 00 	mov    0x160(%esp),%ebx
 8049f12:	8b bc 24 64 01 00 00 	mov    0x164(%esp),%edi
 8049f19:	eb 10                	jmp    8049f2b <_vfprintf_r+0x12bf>
 8049f1b:	90                   	nop
 8049f1c:	83 c0 08             	add    $0x8,%eax
 8049f1f:	83 ed 10             	sub    $0x10,%ebp
 8049f22:	83 fd 10             	cmp    $0x10,%ebp
 8049f25:	0f 8e f9 02 00 00    	jle    804a224 <_vfprintf_r+0x15b8>
 8049f2b:	c7 00 44 18 05 08    	movl   $0x8051844,(%eax)
 8049f31:	c7 40 04 10 00 00 00 	movl   $0x10,0x4(%eax)
 8049f38:	83 84 24 d4 00 00 00 	addl   $0x10,0xd4(%esp)
 8049f3f:	10 
 8049f40:	42                   	inc    %edx
 8049f41:	89 94 24 d0 00 00 00 	mov    %edx,0xd0(%esp)
 8049f48:	83 fa 07             	cmp    $0x7,%edx
 8049f4b:	7e cf                	jle    8049f1c <_vfprintf_r+0x12b0>
 8049f4d:	89 74 24 08          	mov    %esi,0x8(%esp)
 8049f51:	89 7c 24 04          	mov    %edi,0x4(%esp)
 8049f55:	89 1c 24             	mov    %ebx,(%esp)
 8049f58:	e8 8f 53 00 00       	call   804f2ec <__sprint_r>
 8049f5d:	85 c0                	test   %eax,%eax
 8049f5f:	0f 85 6b f3 ff ff    	jne    80492d0 <_vfprintf_r+0x664>
 8049f65:	8d 84 24 00 01 00 00 	lea    0x100(%esp),%eax
 8049f6c:	8b 94 24 d0 00 00 00 	mov    0xd0(%esp),%edx
 8049f73:	eb aa                	jmp    8049f1f <_vfprintf_r+0x12b3>
 8049f75:	8b 5c 24 50          	mov    0x50(%esp),%ebx
 8049f79:	89 fe                	mov    %edi,%esi
 8049f7b:	89 c7                	mov    %eax,%edi
						cp = convbuf + ndig;
					}
#endif
					if (expt < ndig || flags & ALT)
					    PRINT (decimal_point, decp_len);
					PRINTANDPAD (cp, convbuf + ndig,
 8049f7d:	c7 07 44 18 05 08    	movl   $0x8051844,(%edi)
 8049f83:	89 77 04             	mov    %esi,0x4(%edi)
 8049f86:	01 b4 24 d4 00 00 00 	add    %esi,0xd4(%esp)
 8049f8d:	42                   	inc    %edx
 8049f8e:	89 94 24 d0 00 00 00 	mov    %edx,0xd0(%esp)
 8049f95:	83 fa 07             	cmp    $0x7,%edx
 8049f98:	0f 8e 25 f1 ff ff    	jle    80490c3 <_vfprintf_r+0x457>
 8049f9e:	e9 14 fc ff ff       	jmp    8049bb7 <_vfprintf_r+0xf4b>
			/*
			 * ``The result of converting a zero value with an
			 * explicit precision of zero is no characters.''
			 *	-- ANSI X3J11
			 */
			cp = buf + BUF;
 8049fa3:	8d 84 24 00 01 00 00 	lea    0x100(%esp),%eax
					}
#ifdef _WANT_IO_C99_FORMATS
					ndig = 0;
#endif
					do {
					  *--cp = to_char (_uquad % 10);
 8049faa:	89 5c 24 4c          	mov    %ebx,0x4c(%esp)
 8049fae:	89 c3                	mov    %eax,%ebx
 8049fb0:	4b                   	dec    %ebx
 8049fb1:	b8 cd cc cc cc       	mov    $0xcccccccd,%eax
 8049fb6:	f7 e1                	mul    %ecx
 8049fb8:	c1 ea 03             	shr    $0x3,%edx
 8049fbb:	8d 04 92             	lea    (%edx,%edx,4),%eax
 8049fbe:	d1 e0                	shl    %eax
 8049fc0:	29 c1                	sub    %eax,%ecx
 8049fc2:	83 c1 30             	add    $0x30,%ecx
 8049fc5:	88 0b                	mov    %cl,(%ebx)
					       next cases. */
					    if (grouping[1] != '\0')
					      grouping++;
					  }
#endif
					  _uquad /= 10;
 8049fc7:	89 d1                	mov    %edx,%ecx
					} while (_uquad != 0);
 8049fc9:	85 d2                	test   %edx,%edx
 8049fcb:	75 e3                	jne    8049fb0 <_vfprintf_r+0x1344>
 8049fcd:	89 5c 24 60          	mov    %ebx,0x60(%esp)
 8049fd1:	8b 5c 24 4c          	mov    0x4c(%esp),%ebx
 8049fd5:	8b 54 24 44          	mov    0x44(%esp),%edx
 8049fd9:	2b 54 24 60          	sub    0x60(%esp),%edx
 8049fdd:	89 54 24 50          	mov    %edx,0x50(%esp)
 8049fe1:	e9 8a f4 ff ff       	jmp    8049470 <_vfprintf_r+0x804>
					do {
						*--cp = to_char (_uquad & 7);
						_uquad >>= 3;
					} while (_uquad);
					/* handle octal leading 0 */
					if (flags & ALT && *cp != '0')
 8049fe6:	3c 30                	cmp    $0x30,%al
 8049fe8:	0f 84 25 07 00 00    	je     804a713 <_vfprintf_r+0x1aa7>
						*--cp = '0';
 8049fee:	ff 4c 24 60          	decl   0x60(%esp)
 8049ff2:	c6 42 ff 30          	movb   $0x30,-0x1(%edx)
 8049ff6:	8b 54 24 44          	mov    0x44(%esp),%edx
 8049ffa:	2b 54 24 60          	sub    0x60(%esp),%edx
 8049ffe:	89 54 24 50          	mov    %edx,0x50(%esp)
 804a002:	e9 69 f4 ff ff       	jmp    8049470 <_vfprintf_r+0x804>
						PAD (-expt, zeroes);
						PRINT (cp, ndig);
					}
				} else {
					char *convbuf = cp;
					PRINTANDPAD(cp, convbuf + ndig,
 804a007:	8d 84 24 cc 00 00 00 	lea    0xcc(%esp),%eax
 804a00e:	89 44 24 08          	mov    %eax,0x8(%esp)
 804a012:	8b 94 24 64 01 00 00 	mov    0x164(%esp),%edx
 804a019:	89 54 24 04          	mov    %edx,0x4(%esp)
 804a01d:	8b 84 24 60 01 00 00 	mov    0x160(%esp),%eax
 804a024:	89 04 24             	mov    %eax,(%esp)
 804a027:	e8 c0 52 00 00       	call   804f2ec <__sprint_r>
 804a02c:	85 c0                	test   %eax,%eax
 804a02e:	0f 85 9c f2 ff ff    	jne    80492d0 <_vfprintf_r+0x664>
 804a034:	8d bc 24 00 01 00 00 	lea    0x100(%esp),%edi
						    lead, zeroes);
					cp += lead;
 804a03b:	8b 6c 24 60          	mov    0x60(%esp),%ebp
 804a03f:	03 6c 24 78          	add    0x78(%esp),%ebp
					    }
					    if (cp > convbuf + ndig)
						cp = convbuf + ndig;
					}
#endif
					if (expt < ndig || flags & ALT)
 804a043:	8b 84 24 b0 00 00 00 	mov    0xb0(%esp),%eax
 804a04a:	3b 44 24 70          	cmp    0x70(%esp),%eax
 804a04e:	0f 8c e9 00 00 00    	jl     804a13d <_vfprintf_r+0x14d1>
 804a054:	f6 44 24 48 01       	testb  $0x1,0x48(%esp)
 804a059:	0f 85 de 00 00 00    	jne    804a13d <_vfprintf_r+0x14d1>
					    PRINT (decimal_point, decp_len);
					PRINTANDPAD (cp, convbuf + ndig,
 804a05f:	8b 54 24 50          	mov    0x50(%esp),%edx
 804a063:	29 ea                	sub    %ebp,%edx
 804a065:	8b 4c 24 70          	mov    0x70(%esp),%ecx
 804a069:	29 c1                	sub    %eax,%ecx
 804a06b:	89 c8                	mov    %ecx,%eax
 804a06d:	89 ce                	mov    %ecx,%esi
 804a06f:	39 d1                	cmp    %edx,%ecx
 804a071:	7e 02                	jle    804a075 <_vfprintf_r+0x1409>
 804a073:	89 d6                	mov    %edx,%esi
 804a075:	85 f6                	test   %esi,%esi
 804a077:	7e 27                	jle    804a0a0 <_vfprintf_r+0x1434>
 804a079:	89 2f                	mov    %ebp,(%edi)
 804a07b:	89 77 04             	mov    %esi,0x4(%edi)
 804a07e:	01 b4 24 d4 00 00 00 	add    %esi,0xd4(%esp)
 804a085:	8b 94 24 d0 00 00 00 	mov    0xd0(%esp),%edx
 804a08c:	42                   	inc    %edx
 804a08d:	89 94 24 d0 00 00 00 	mov    %edx,0xd0(%esp)
 804a094:	83 fa 07             	cmp    $0x7,%edx
 804a097:	0f 8f b7 07 00 00    	jg     804a854 <_vfprintf_r+0x1be8>
 804a09d:	83 c7 08             	add    $0x8,%edi
 804a0a0:	85 f6                	test   %esi,%esi
 804a0a2:	0f 88 a5 07 00 00    	js     804a84d <_vfprintf_r+0x1be1>
 804a0a8:	29 f0                	sub    %esi,%eax
 804a0aa:	89 c6                	mov    %eax,%esi
 804a0ac:	85 c0                	test   %eax,%eax
 804a0ae:	0f 8e 12 f0 ff ff    	jle    80490c6 <_vfprintf_r+0x45a>
 804a0b4:	8b 94 24 d0 00 00 00 	mov    0xd0(%esp),%edx
 804a0bb:	83 f8 10             	cmp    $0x10,%eax
 804a0be:	0f 8e b9 fe ff ff    	jle    8049f7d <_vfprintf_r+0x1311>
 804a0c4:	8d ac 24 cc 00 00 00 	lea    0xcc(%esp),%ebp
 804a0cb:	89 5c 24 50          	mov    %ebx,0x50(%esp)
 804a0cf:	89 f8                	mov    %edi,%eax
 804a0d1:	89 f7                	mov    %esi,%edi
 804a0d3:	8b 9c 24 60 01 00 00 	mov    0x160(%esp),%ebx
 804a0da:	8b b4 24 64 01 00 00 	mov    0x164(%esp),%esi
 804a0e1:	eb 10                	jmp    804a0f3 <_vfprintf_r+0x1487>
 804a0e3:	90                   	nop
 804a0e4:	83 c0 08             	add    $0x8,%eax
 804a0e7:	83 ef 10             	sub    $0x10,%edi
 804a0ea:	83 ff 10             	cmp    $0x10,%edi
 804a0ed:	0f 8e 82 fe ff ff    	jle    8049f75 <_vfprintf_r+0x1309>
 804a0f3:	c7 00 44 18 05 08    	movl   $0x8051844,(%eax)
 804a0f9:	c7 40 04 10 00 00 00 	movl   $0x10,0x4(%eax)
 804a100:	83 84 24 d4 00 00 00 	addl   $0x10,0xd4(%esp)
 804a107:	10 
 804a108:	42                   	inc    %edx
 804a109:	89 94 24 d0 00 00 00 	mov    %edx,0xd0(%esp)
 804a110:	83 fa 07             	cmp    $0x7,%edx
 804a113:	7e cf                	jle    804a0e4 <_vfprintf_r+0x1478>
 804a115:	89 6c 24 08          	mov    %ebp,0x8(%esp)
 804a119:	89 74 24 04          	mov    %esi,0x4(%esp)
 804a11d:	89 1c 24             	mov    %ebx,(%esp)
 804a120:	e8 c7 51 00 00       	call   804f2ec <__sprint_r>
 804a125:	85 c0                	test   %eax,%eax
 804a127:	0f 85 a3 f1 ff ff    	jne    80492d0 <_vfprintf_r+0x664>
 804a12d:	8d 84 24 00 01 00 00 	lea    0x100(%esp),%eax
 804a134:	8b 94 24 d0 00 00 00 	mov    0xd0(%esp),%edx
 804a13b:	eb aa                	jmp    804a0e7 <_vfprintf_r+0x147b>
					    if (cp > convbuf + ndig)
						cp = convbuf + ndig;
					}
#endif
					if (expt < ndig || flags & ALT)
					    PRINT (decimal_point, decp_len);
 804a13d:	8b 4c 24 74          	mov    0x74(%esp),%ecx
 804a141:	89 0f                	mov    %ecx,(%edi)
 804a143:	8b 54 24 7c          	mov    0x7c(%esp),%edx
 804a147:	89 57 04             	mov    %edx,0x4(%edi)
 804a14a:	01 94 24 d4 00 00 00 	add    %edx,0xd4(%esp)
 804a151:	8b 94 24 d0 00 00 00 	mov    0xd0(%esp),%edx
 804a158:	42                   	inc    %edx
 804a159:	89 94 24 d0 00 00 00 	mov    %edx,0xd0(%esp)
 804a160:	83 fa 07             	cmp    $0x7,%edx
 804a163:	0f 8f a4 06 00 00    	jg     804a80d <_vfprintf_r+0x1ba1>
 804a169:	83 c7 08             	add    $0x8,%edi
 804a16c:	e9 ee fe ff ff       	jmp    804a05f <_vfprintf_r+0x13f3>
					PRINTANDPAD (cp, convbuf + ndig,
						     ndig - expt, zeroes);
				}
			} else {	/* 'a', 'A', 'e', or 'E' */
				if (ndig > 1 || flags & ALT) {
 804a171:	f6 44 24 48 01       	testb  $0x1,0x48(%esp)
 804a176:	0f 85 6b f9 ff ff    	jne    8049ae7 <_vfprintf_r+0xe7b>
						PRINT (cp, ndig - 1);
					} else	/* 0.[0..] */
						/* __dtoa irregularity */
						PAD (ndig - 1, zeroes);
				} else	/* XeYYY */
					PRINT (cp, 1);
 804a17c:	89 07                	mov    %eax,(%edi)
 804a17e:	c7 47 04 01 00 00 00 	movl   $0x1,0x4(%edi)
 804a185:	ff 84 24 d4 00 00 00 	incl   0xd4(%esp)
 804a18c:	8b 94 24 d0 00 00 00 	mov    0xd0(%esp),%edx
 804a193:	42                   	inc    %edx
 804a194:	89 94 24 d0 00 00 00 	mov    %edx,0xd0(%esp)
 804a19b:	83 fa 07             	cmp    $0x7,%edx
 804a19e:	0f 8e e5 f9 ff ff    	jle    8049b89 <_vfprintf_r+0xf1d>
				if (ndig > 1 || flags & ALT) {
					PRINT (cp, 1);
					cp++;
					PRINT (decimal_point, decp_len);
					if (_fpvalue) {
						PRINT (cp, ndig - 1);
 804a1a4:	8d 84 24 cc 00 00 00 	lea    0xcc(%esp),%eax
 804a1ab:	89 44 24 08          	mov    %eax,0x8(%esp)
 804a1af:	8b 94 24 64 01 00 00 	mov    0x164(%esp),%edx
 804a1b6:	89 54 24 04          	mov    %edx,0x4(%esp)
 804a1ba:	8b 84 24 60 01 00 00 	mov    0x160(%esp),%eax
 804a1c1:	89 04 24             	mov    %eax,(%esp)
 804a1c4:	e8 23 51 00 00       	call   804f2ec <__sprint_r>
 804a1c9:	85 c0                	test   %eax,%eax
 804a1cb:	0f 85 ff f0 ff ff    	jne    80492d0 <_vfprintf_r+0x664>
 804a1d1:	8d bc 24 00 01 00 00 	lea    0x100(%esp),%edi
 804a1d8:	8b 94 24 d0 00 00 00 	mov    0xd0(%esp),%edx
 804a1df:	e9 a8 f9 ff ff       	jmp    8049b8c <_vfprintf_r+0xf20>
					PRINTANDPAD (cp, convbuf + ndig,
						     ndig - expt, zeroes);
				}
			} else {	/* 'a', 'A', 'e', or 'E' */
				if (ndig > 1 || flags & ALT) {
					PRINT (cp, 1);
 804a1e4:	8d 84 24 cc 00 00 00 	lea    0xcc(%esp),%eax
 804a1eb:	89 44 24 08          	mov    %eax,0x8(%esp)
 804a1ef:	8b 94 24 64 01 00 00 	mov    0x164(%esp),%edx
 804a1f6:	89 54 24 04          	mov    %edx,0x4(%esp)
 804a1fa:	8b 84 24 60 01 00 00 	mov    0x160(%esp),%eax
 804a201:	89 04 24             	mov    %eax,(%esp)
 804a204:	e8 e3 50 00 00       	call   804f2ec <__sprint_r>
 804a209:	85 c0                	test   %eax,%eax
 804a20b:	0f 85 bf f0 ff ff    	jne    80492d0 <_vfprintf_r+0x664>
 804a211:	8d bc 24 00 01 00 00 	lea    0x100(%esp),%edi
 804a218:	8b 94 24 d0 00 00 00 	mov    0xd0(%esp),%edx
 804a21f:	e9 ee f8 ff ff       	jmp    8049b12 <_vfprintf_r+0xea6>
 804a224:	8b 5c 24 50          	mov    0x50(%esp),%ebx
 804a228:	89 c7                	mov    %eax,%edi
					PRINT (decimal_point, decp_len);
					if (_fpvalue) {
						PRINT (cp, ndig - 1);
					} else	/* 0.[0..] */
						/* __dtoa irregularity */
						PAD (ndig - 1, zeroes);
 804a22a:	c7 07 44 18 05 08    	movl   $0x8051844,(%edi)
 804a230:	89 6f 04             	mov    %ebp,0x4(%edi)
 804a233:	01 ac 24 d4 00 00 00 	add    %ebp,0xd4(%esp)
 804a23a:	42                   	inc    %edx
 804a23b:	89 94 24 d0 00 00 00 	mov    %edx,0xd0(%esp)
 804a242:	83 fa 07             	cmp    $0x7,%edx
 804a245:	0f 8e 3e f9 ff ff    	jle    8049b89 <_vfprintf_r+0xf1d>
 804a24b:	e9 54 ff ff ff       	jmp    804a1a4 <_vfprintf_r+0x1538>
				}
			} else {	/* 'a', 'A', 'e', or 'E' */
				if (ndig > 1 || flags & ALT) {
					PRINT (cp, 1);
					cp++;
					PRINT (decimal_point, decp_len);
 804a250:	8d 84 24 cc 00 00 00 	lea    0xcc(%esp),%eax
 804a257:	89 44 24 08          	mov    %eax,0x8(%esp)
 804a25b:	8b 94 24 64 01 00 00 	mov    0x164(%esp),%edx
 804a262:	89 54 24 04          	mov    %edx,0x4(%esp)
 804a266:	8b 84 24 60 01 00 00 	mov    0x160(%esp),%eax
 804a26d:	89 04 24             	mov    %eax,(%esp)
 804a270:	e8 77 50 00 00       	call   804f2ec <__sprint_r>
 804a275:	85 c0                	test   %eax,%eax
 804a277:	0f 85 53 f0 ff ff    	jne    80492d0 <_vfprintf_r+0x664>
 804a27d:	8d bc 24 00 01 00 00 	lea    0x100(%esp),%edi
 804a284:	8b 94 24 d0 00 00 00 	mov    0xd0(%esp),%edx
 804a28b:	e9 aa f8 ff ff       	jmp    8049b3a <_vfprintf_r+0xece>
		if (flags & HEXPREFIX)
			PRINT (ox, 2);

		/* right-adjusting zero padding */
		if ((flags & (LADJUST|ZEROPAD)) == ZEROPAD)
			PAD (width - realsz, zeroes);
 804a290:	8d 84 24 cc 00 00 00 	lea    0xcc(%esp),%eax
 804a297:	89 44 24 08          	mov    %eax,0x8(%esp)
 804a29b:	8b 94 24 64 01 00 00 	mov    0x164(%esp),%edx
 804a2a2:	89 54 24 04          	mov    %edx,0x4(%esp)
 804a2a6:	8b 84 24 60 01 00 00 	mov    0x160(%esp),%eax
 804a2ad:	89 04 24             	mov    %eax,(%esp)
 804a2b0:	e8 37 50 00 00       	call   804f2ec <__sprint_r>
 804a2b5:	85 c0                	test   %eax,%eax
 804a2b7:	0f 85 13 f0 ff ff    	jne    80492d0 <_vfprintf_r+0x664>
 804a2bd:	8d bc 24 00 01 00 00 	lea    0x100(%esp),%edi
 804a2c4:	e9 fc ec ff ff       	jmp    8048fc5 <_vfprintf_r+0x359>
#else
			if ((long) _uquad < 0)
#endif
			{

				_uquad = -_uquad;
 804a2c9:	f7 d9                	neg    %ecx
				sign = '-';
 804a2cb:	c6 84 24 ad 00 00 00 	movb   $0x2d,0xad(%esp)
 804a2d2:	2d 
 804a2d3:	be 2d 00 00 00       	mov    $0x2d,%esi
			}
			base = DEC;
 804a2d8:	b0 01                	mov    $0x1,%al
 804a2da:	e9 43 f1 ff ff       	jmp    8049422 <_vfprintf_r+0x7b6>
					cp = "inf";
				size = 3;
				flags &= ~ZEROPAD;
				break;
			}
			if (isnan (_fpvalue)) {
 804a2df:	dd 84 24 80 00 00 00 	fldl   0x80(%esp)
 804a2e6:	dd 1c 24             	fstpl  (%esp)
 804a2e9:	e8 16 4e 00 00       	call   804f104 <__fpclassifyd>
 804a2ee:	85 c0                	test   %eax,%eax
 804a2f0:	0f 84 4f 03 00 00    	je     804a645 <_vfprintf_r+0x19d9>
				  }
				else
				  cp = buf;
			} else
# endif /* _WANT_IO_C99_FORMATS */
			if (prec == -1) {
 804a2f6:	83 fd ff             	cmp    $0xffffffff,%ebp
 804a2f9:	0f 84 bf 07 00 00    	je     804aabe <_vfprintf_r+0x1e52>
				prec = DEFPREC;
			} else if ((ch == 'g' || ch == 'G') && prec == 0) {
 804a2ff:	83 7c 24 68 67       	cmpl   $0x67,0x68(%esp)
 804a304:	0f 84 5b 07 00 00    	je     804aa65 <_vfprintf_r+0x1df9>
 804a30a:	83 7c 24 68 47       	cmpl   $0x47,0x68(%esp)
 804a30f:	0f 84 50 07 00 00    	je     804aa65 <_vfprintf_r+0x1df9>
				prec = 1;
			}

			flags |= FPT;
 804a315:	8b 4c 24 48          	mov    0x48(%esp),%ecx
 804a319:	80 cd 01             	or     $0x1,%ch
 804a31c:	89 4c 24 58          	mov    %ecx,0x58(%esp)
	int mode, dsgn;
	char *digits, *bp, *rve;
# ifdef _NO_LONGDBL
	union double_union tmp;

	tmp.d = value;
 804a320:	8b 94 24 84 00 00 00 	mov    0x84(%esp),%edx
	if (word0 (tmp) & Sign_bit) { /* this will check for < 0 and -0.0 */
		value = -value;
 804a327:	dd 84 24 80 00 00 00 	fldl   0x80(%esp)
	char *digits, *bp, *rve;
# ifdef _NO_LONGDBL
	union double_union tmp;

	tmp.d = value;
	if (word0 (tmp) & Sign_bit) { /* this will check for < 0 and -0.0 */
 804a32e:	85 d2                	test   %edx,%edx
 804a330:	0f 88 92 07 00 00    	js     804aac8 <_vfprintf_r+0x1e5c>
		value = -value;
		*sign = '-';
	} else
		*sign = '\000';
 804a336:	c6 44 24 64 00       	movb   $0x0,0x64(%esp)
		}
		*length = bp - buf;
		return buf;
	}
# endif /* _WANT_IO_C99_FORMATS */
	if (ch == 'f' || ch == 'F') {
 804a33b:	83 7c 24 68 66       	cmpl   $0x66,0x68(%esp)
 804a340:	0f 84 b2 06 00 00    	je     804a9f8 <_vfprintf_r+0x1d8c>
 804a346:	83 7c 24 68 46       	cmpl   $0x46,0x68(%esp)
 804a34b:	0f 84 a7 06 00 00    	je     804a9f8 <_vfprintf_r+0x1d8c>
	} else {
		/* To obtain ndigits after the decimal point for the 'e'
		 * and 'E' formats, round to ndigits + 1 significant
		 * figures.
		 */
		if (ch == 'e' || ch == 'E') {
 804a351:	83 7c 24 68 65       	cmpl   $0x65,0x68(%esp)
 804a356:	0f 84 78 07 00 00    	je     804aad4 <_vfprintf_r+0x1e68>
 804a35c:	83 7c 24 68 45       	cmpl   $0x45,0x68(%esp)
 804a361:	0f 84 6d 07 00 00    	je     804aad4 <_vfprintf_r+0x1e68>
 804a367:	89 ee                	mov    %ebp,%esi
			ndigits++;
		}
		mode = 2;		/* ndigits significant digits */
	}

	digits = _DTOA_R (data, value, mode, ndigits, decpt, &dsgn, &rve);
 804a369:	8d 84 24 b8 00 00 00 	lea    0xb8(%esp),%eax
 804a370:	89 44 24 1c          	mov    %eax,0x1c(%esp)
 804a374:	8d 84 24 b4 00 00 00 	lea    0xb4(%esp),%eax
 804a37b:	89 44 24 18          	mov    %eax,0x18(%esp)
				prec = 1;
			}

			flags |= FPT;

			cp = cvt (data, _fpvalue, prec, flags, &softsign,
 804a37f:	8d 84 24 b0 00 00 00 	lea    0xb0(%esp),%eax
 804a386:	89 44 24 14          	mov    %eax,0x14(%esp)
			ndigits++;
		}
		mode = 2;		/* ndigits significant digits */
	}

	digits = _DTOA_R (data, value, mode, ndigits, decpt, &dsgn, &rve);
 804a38a:	89 74 24 10          	mov    %esi,0x10(%esp)
 804a38e:	c7 44 24 0c 02 00 00 	movl   $0x2,0xc(%esp)
 804a395:	00 
 804a396:	dd 54 24 04          	fstl   0x4(%esp)
 804a39a:	8b 84 24 60 01 00 00 	mov    0x160(%esp),%eax
 804a3a1:	89 04 24             	mov    %eax,(%esp)
 804a3a4:	dd 5c 24 20          	fstpl  0x20(%esp)
 804a3a8:	e8 4b 0e 00 00       	call   804b1f8 <_dtoa_r>
 804a3ad:	89 44 24 60          	mov    %eax,0x60(%esp)

	if ((ch != 'g' && ch != 'G') || flags & ALT) {	/* Print trailing zeros */
 804a3b1:	83 7c 24 68 67       	cmpl   $0x67,0x68(%esp)
 804a3b6:	dd 44 24 20          	fldl   0x20(%esp)
 804a3ba:	74 07                	je     804a3c3 <_vfprintf_r+0x1757>
 804a3bc:	83 7c 24 68 47       	cmpl   $0x47,0x68(%esp)
 804a3c1:	75 0b                	jne    804a3ce <_vfprintf_r+0x1762>
 804a3c3:	f6 44 24 48 01       	testb  $0x1,0x48(%esp)
 804a3c8:	0f 84 b2 07 00 00    	je     804ab80 <_vfprintf_r+0x1f14>
		bp = digits + ndigits;
 804a3ce:	8b 54 24 60          	mov    0x60(%esp),%edx
 804a3d2:	01 f2                	add    %esi,%edx
		if (ch == 'f' || ch == 'F') {
 804a3d4:	83 7c 24 68 46       	cmpl   $0x46,0x68(%esp)
 804a3d9:	0f 84 6b 06 00 00    	je     804aa4a <_vfprintf_r+0x1dde>
			if (*digits == '0' && value)
				*decpt = -ndigits + 1;
			bp += *decpt;
		}
		if (value == 0)	/* kludge for __dtoa irregularity */
 804a3df:	d9 ee                	fldz   
 804a3e1:	d9 c9                	fxch   %st(1)
 804a3e3:	da e9                	fucompp 
 804a3e5:	df e0                	fnstsw %ax
 804a3e7:	80 e4 45             	and    $0x45,%ah
 804a3ea:	80 fc 40             	cmp    $0x40,%ah
 804a3ed:	74 20                	je     804a40f <_vfprintf_r+0x17a3>
			rve = bp;
		while (rve < bp)
 804a3ef:	8b 84 24 b8 00 00 00 	mov    0xb8(%esp),%eax
 804a3f6:	39 c2                	cmp    %eax,%edx
 804a3f8:	0f 86 c7 08 00 00    	jbe    804acc5 <_vfprintf_r+0x2059>
 804a3fe:	66 90                	xchg   %ax,%ax
			*rve++ = '0';
 804a400:	c6 00 30             	movb   $0x30,(%eax)
 804a403:	40                   	inc    %eax
 804a404:	89 84 24 b8 00 00 00 	mov    %eax,0xb8(%esp)
				*decpt = -ndigits + 1;
			bp += *decpt;
		}
		if (value == 0)	/* kludge for __dtoa irregularity */
			rve = bp;
		while (rve < bp)
 804a40b:	39 d0                	cmp    %edx,%eax
 804a40d:	75 f1                	jne    804a400 <_vfprintf_r+0x1794>
			*rve++ = '0';
	}
	*length = rve - digits;
 804a40f:	2b 54 24 60          	sub    0x60(%esp),%edx
 804a413:	89 54 24 70          	mov    %edx,0x70(%esp)
			flags |= FPT;

			cp = cvt (data, _fpvalue, prec, flags, &softsign,
				  &expt, ch, &ndig, cp);

			if (ch == 'g' || ch == 'G') {
 804a417:	83 7c 24 68 67       	cmpl   $0x67,0x68(%esp)
 804a41c:	0f 84 fe 04 00 00    	je     804a920 <_vfprintf_r+0x1cb4>
 804a422:	83 7c 24 68 47       	cmpl   $0x47,0x68(%esp)
 804a427:	0f 84 f3 04 00 00    	je     804a920 <_vfprintf_r+0x1cb4>
			}
# ifdef _WANT_IO_C99_FORMATS
			else if (ch == 'F')
				ch = 'f';
# endif
			if (ch <= 'e') {	/* 'a', 'A', 'e', or 'E' fmt */
 804a42d:	83 7c 24 68 66       	cmpl   $0x66,0x68(%esp)
 804a432:	0f 85 94 08 00 00    	jne    804accc <_vfprintf_r+0x2060>
# ifdef _WANT_IO_C99_FORMATS
				flags &= ~GROUPING;
# endif
			} else {
				if (ch == 'f') {		/* f fmt */
					if (expt > 0) {
 804a438:	8b 84 24 b0 00 00 00 	mov    0xb0(%esp),%eax
 804a43f:	85 c0                	test   %eax,%eax
 804a441:	0f 8e c0 07 00 00    	jle    804ac07 <_vfprintf_r+0x1f9b>
						size = expt;
						if (prec || flags & ALT)
 804a447:	85 ed                	test   %ebp,%ebp
 804a449:	0f 85 4c 07 00 00    	jne    804ab9b <_vfprintf_r+0x1f2f>
 804a44f:	f6 44 24 48 01       	testb  $0x1,0x48(%esp)
 804a454:	0f 85 41 07 00 00    	jne    804ab9b <_vfprintf_r+0x1f2f>
 804a45a:	89 44 24 4c          	mov    %eax,0x4c(%esp)
 804a45e:	89 44 24 50          	mov    %eax,0x50(%esp)
							nrepeats++;
					}
					size += (nseps + nrepeats) * thsnd_len;
				} else
# endif
					lead = expt;
 804a462:	89 44 24 78          	mov    %eax,0x78(%esp)
			}

			if (softsign)
 804a466:	80 7c 24 64 00       	cmpb   $0x0,0x64(%esp)
 804a46b:	0f 85 6e 04 00 00    	jne    804a8df <_vfprintf_r+0x1c73>
 804a471:	0f b6 b4 24 ad 00 00 	movzbl 0xad(%esp),%esi
 804a478:	00 
				prec = DEFPREC;
			} else if ((ch == 'g' || ch == 'G') && prec == 0) {
				prec = 1;
			}

			flags |= FPT;
 804a479:	8b 4c 24 58          	mov    0x58(%esp),%ecx
 804a47d:	89 4c 24 48          	mov    %ecx,0x48(%esp)
#endif
		fmt_anchor = fmt;
		fmt++;		/* skip over '%' */

		flags = 0;
		dprec = 0;
 804a481:	c7 44 24 6c 00 00 00 	movl   $0x0,0x6c(%esp)
 804a488:	00 
 804a489:	e9 fe ef ff ff       	jmp    804948c <_vfprintf_r+0x820>
		case 'G':
# ifdef _NO_LONGDBL
			if (flags & LONGDBL) {
				_fpvalue = (double) GET_ARG (N, ap, _LONG_DOUBLE);
			} else {
				_fpvalue = GET_ARG (N, ap, double);
 804a48e:	dd 01                	fldl   (%ecx)
 804a490:	dd 9c 24 80 00 00 00 	fstpl  0x80(%esp)
 804a497:	83 c1 08             	add    $0x8,%ecx
 804a49a:	89 8c 24 6c 01 00 00 	mov    %ecx,0x16c(%esp)
 804a4a1:	e9 71 f1 ff ff       	jmp    8049617 <_vfprintf_r+0x9ab>
		case 'X':
			xdigs = "0123456789ABCDEF";
			goto hex;
		case 'x':
			xdigs = "0123456789abcdef";
hex:			_uquad = UARG ();
 804a4a6:	8b 94 24 6c 01 00 00 	mov    0x16c(%esp),%edx
 804a4ad:	8b 0a                	mov    (%edx),%ecx
 804a4af:	83 c2 04             	add    $0x4,%edx
 804a4b2:	89 94 24 6c 01 00 00 	mov    %edx,0x16c(%esp)
 804a4b9:	e9 6a f0 ff ff       	jmp    8049528 <_vfprintf_r+0x8bc>
			continue;	/* no output */
		case 'O': /* extension */
			flags |= LONGINT;
			/*FALLTHROUGH*/
		case 'o':
			_uquad = UARG ();
 804a4be:	8b 94 24 6c 01 00 00 	mov    0x16c(%esp),%edx
 804a4c5:	8b 0a                	mov    (%edx),%ecx
 804a4c7:	83 c2 04             	add    $0x4,%edx
 804a4ca:	89 94 24 6c 01 00 00 	mov    %edx,0x16c(%esp)
			base = OCT;
 804a4d1:	31 c0                	xor    %eax,%eax
 804a4d3:	e9 40 ef ff ff       	jmp    8049418 <_vfprintf_r+0x7ac>
 804a4d8:	8b 5c 24 58          	mov    0x58(%esp),%ebx
 804a4dc:	89 fe                	mov    %edi,%esi
 804a4de:	89 c7                	mov    %eax,%edi
						PAD (-expt, zeroes);
						PRINT (cp, ndig);
					}
				} else {
					char *convbuf = cp;
					PRINTANDPAD(cp, convbuf + ndig,
 804a4e0:	c7 07 44 18 05 08    	movl   $0x8051844,(%edi)
 804a4e6:	89 77 04             	mov    %esi,0x4(%edi)
 804a4e9:	01 b4 24 d4 00 00 00 	add    %esi,0xd4(%esp)
 804a4f0:	42                   	inc    %edx
 804a4f1:	89 94 24 d0 00 00 00 	mov    %edx,0xd0(%esp)
 804a4f8:	83 fa 07             	cmp    $0x7,%edx
 804a4fb:	0f 8f 06 fb ff ff    	jg     804a007 <_vfprintf_r+0x139b>
 804a501:	83 c7 08             	add    $0x8,%edi
 804a504:	e9 32 fb ff ff       	jmp    804a03b <_vfprintf_r+0x13cf>
			PRINT (cp, size);
		} else {	/* glue together f_p fragments */
			if (ch >= 'f') {	/* 'f' or 'g' */
				if (_fpvalue == 0) {
					/* kludge for __dtoa irregularity */
					PRINT ("0", 1);
 804a509:	8d 84 24 cc 00 00 00 	lea    0xcc(%esp),%eax
 804a510:	89 44 24 08          	mov    %eax,0x8(%esp)
 804a514:	8b 94 24 64 01 00 00 	mov    0x164(%esp),%edx
 804a51b:	89 54 24 04          	mov    %edx,0x4(%esp)
 804a51f:	8b 84 24 60 01 00 00 	mov    0x160(%esp),%eax
 804a526:	89 04 24             	mov    %eax,(%esp)
 804a529:	e8 be 4d 00 00       	call   804f2ec <__sprint_r>
 804a52e:	85 c0                	test   %eax,%eax
 804a530:	0f 85 9a ed ff ff    	jne    80492d0 <_vfprintf_r+0x664>
 804a536:	8d bc 24 00 01 00 00 	lea    0x100(%esp),%edi
 804a53d:	e9 e9 f3 ff ff       	jmp    804992b <_vfprintf_r+0xcbf>
					if (expt < ndig || flags & ALT) {
						PRINT (decimal_point, decp_len);
						PAD (ndig - 1, zeroes);
					}
				} else if (expt <= 0) {
					PRINT ("0", 1);
 804a542:	c7 07 cd 16 05 08    	movl   $0x80516cd,(%edi)
 804a548:	c7 47 04 01 00 00 00 	movl   $0x1,0x4(%edi)
 804a54f:	ff 84 24 d4 00 00 00 	incl   0xd4(%esp)
 804a556:	8b 94 24 d0 00 00 00 	mov    0xd0(%esp),%edx
 804a55d:	42                   	inc    %edx
 804a55e:	89 94 24 d0 00 00 00 	mov    %edx,0xd0(%esp)
 804a565:	83 fa 07             	cmp    $0x7,%edx
 804a568:	0f 8f 2c 01 00 00    	jg     804a69a <_vfprintf_r+0x1a2e>
 804a56e:	83 c7 08             	add    $0x8,%edi
					if (expt || ndig || flags & ALT) {
 804a571:	85 f6                	test   %esi,%esi
 804a573:	75 13                	jne    804a588 <_vfprintf_r+0x191c>
 804a575:	8b 54 24 70          	mov    0x70(%esp),%edx
 804a579:	85 d2                	test   %edx,%edx
 804a57b:	75 0b                	jne    804a588 <_vfprintf_r+0x191c>
 804a57d:	f6 44 24 48 01       	testb  $0x1,0x48(%esp)
 804a582:	0f 84 3e eb ff ff    	je     80490c6 <_vfprintf_r+0x45a>
						PRINT (decimal_point, decp_len);
 804a588:	8b 4c 24 74          	mov    0x74(%esp),%ecx
 804a58c:	89 0f                	mov    %ecx,(%edi)
 804a58e:	8b 44 24 7c          	mov    0x7c(%esp),%eax
 804a592:	89 47 04             	mov    %eax,0x4(%edi)
 804a595:	01 84 24 d4 00 00 00 	add    %eax,0xd4(%esp)
 804a59c:	8b 94 24 d0 00 00 00 	mov    0xd0(%esp),%edx
 804a5a3:	42                   	inc    %edx
 804a5a4:	89 94 24 d0 00 00 00 	mov    %edx,0xd0(%esp)
 804a5ab:	83 fa 07             	cmp    $0x7,%edx
 804a5ae:	0f 8f c3 04 00 00    	jg     804aa77 <_vfprintf_r+0x1e0b>
 804a5b4:	83 c7 08             	add    $0x8,%edi
						PAD (-expt, zeroes);
 804a5b7:	f7 de                	neg    %esi
 804a5b9:	85 f6                	test   %esi,%esi
 804a5bb:	0f 8e cf 01 00 00    	jle    804a790 <_vfprintf_r+0x1b24>
 804a5c1:	83 fe 10             	cmp    $0x10,%esi
 804a5c4:	0f 8e 1d 02 00 00    	jle    804a7e7 <_vfprintf_r+0x1b7b>
 804a5ca:	8d ac 24 cc 00 00 00 	lea    0xcc(%esp),%ebp
 804a5d1:	89 5c 24 50          	mov    %ebx,0x50(%esp)
 804a5d5:	89 f8                	mov    %edi,%eax
 804a5d7:	89 f7                	mov    %esi,%edi
 804a5d9:	8b 9c 24 60 01 00 00 	mov    0x160(%esp),%ebx
 804a5e0:	8b b4 24 64 01 00 00 	mov    0x164(%esp),%esi
 804a5e7:	eb 12                	jmp    804a5fb <_vfprintf_r+0x198f>
 804a5e9:	8d 76 00             	lea    0x0(%esi),%esi
 804a5ec:	83 c0 08             	add    $0x8,%eax
 804a5ef:	83 ef 10             	sub    $0x10,%edi
 804a5f2:	83 ff 10             	cmp    $0x10,%edi
 804a5f5:	0f 8e e4 01 00 00    	jle    804a7df <_vfprintf_r+0x1b73>
 804a5fb:	c7 00 44 18 05 08    	movl   $0x8051844,(%eax)
 804a601:	c7 40 04 10 00 00 00 	movl   $0x10,0x4(%eax)
 804a608:	83 84 24 d4 00 00 00 	addl   $0x10,0xd4(%esp)
 804a60f:	10 
 804a610:	42                   	inc    %edx
 804a611:	89 94 24 d0 00 00 00 	mov    %edx,0xd0(%esp)
 804a618:	83 fa 07             	cmp    $0x7,%edx
 804a61b:	7e cf                	jle    804a5ec <_vfprintf_r+0x1980>
 804a61d:	89 6c 24 08          	mov    %ebp,0x8(%esp)
 804a621:	89 74 24 04          	mov    %esi,0x4(%esp)
 804a625:	89 1c 24             	mov    %ebx,(%esp)
 804a628:	e8 bf 4c 00 00       	call   804f2ec <__sprint_r>
 804a62d:	85 c0                	test   %eax,%eax
 804a62f:	0f 85 9b ec ff ff    	jne    80492d0 <_vfprintf_r+0x664>
 804a635:	8d 84 24 00 01 00 00 	lea    0x100(%esp),%eax
 804a63c:	8b 94 24 d0 00 00 00 	mov    0xd0(%esp),%edx
 804a643:	eb aa                	jmp    804a5ef <_vfprintf_r+0x1983>
				size = 3;
				flags &= ~ZEROPAD;
				break;
			}
			if (isnan (_fpvalue)) {
				if (ch <= 'G') /* 'A', 'E', 'F', or 'G' */
 804a645:	83 7c 24 68 47       	cmpl   $0x47,0x68(%esp)
 804a64a:	0f 8f b1 02 00 00    	jg     804a901 <_vfprintf_r+0x1c95>
					cp = "NAN";
 804a650:	c7 44 24 60 9c 16 05 	movl   $0x805169c,0x60(%esp)
 804a657:	08 
				else
					cp = "nan";
				size = 3;
				flags &= ~ZEROPAD;
 804a658:	81 64 24 48 7f ff ff 	andl   $0xffffff7f,0x48(%esp)
 804a65f:	ff 
 804a660:	0f b6 b4 24 ad 00 00 	movzbl 0xad(%esp),%esi
 804a667:	00 
				break;
 804a668:	c7 44 24 4c 03 00 00 	movl   $0x3,0x4c(%esp)
 804a66f:	00 
			if (isnan (_fpvalue)) {
				if (ch <= 'G') /* 'A', 'E', 'F', or 'G' */
					cp = "NAN";
				else
					cp = "nan";
				size = 3;
 804a670:	c7 44 24 50 03 00 00 	movl   $0x3,0x50(%esp)
 804a677:	00 
#endif
		fmt_anchor = fmt;
		fmt++;		/* skip over '%' */

		flags = 0;
		dprec = 0;
 804a678:	c7 44 24 6c 00 00 00 	movl   $0x0,0x6c(%esp)
 804a67f:	00 
		width = 0;
		prec = -1;
		sign = '\0';
#ifdef FLOATING_POINT
		lead = 0;
 804a680:	c7 44 24 78 00 00 00 	movl   $0x0,0x78(%esp)
 804a687:	00 
					cp = "NAN";
				else
					cp = "nan";
				size = 3;
				flags &= ~ZEROPAD;
				break;
 804a688:	e9 ff ed ff ff       	jmp    804948c <_vfprintf_r+0x820>
			 */
			if (isinf (_fpvalue)) {
				if (_fpvalue < 0)
					sign = '-';
				if (ch <= 'G') /* 'A', 'E', 'F', or 'G' */
					cp = "INF";
 804a68d:	c7 44 24 60 94 16 05 	movl   $0x8051694,0x60(%esp)
 804a694:	08 
 804a695:	e9 c6 ef ff ff       	jmp    8049660 <_vfprintf_r+0x9f4>
					if (expt < ndig || flags & ALT) {
						PRINT (decimal_point, decp_len);
						PAD (ndig - 1, zeroes);
					}
				} else if (expt <= 0) {
					PRINT ("0", 1);
 804a69a:	8d 84 24 cc 00 00 00 	lea    0xcc(%esp),%eax
 804a6a1:	89 44 24 08          	mov    %eax,0x8(%esp)
 804a6a5:	8b 94 24 64 01 00 00 	mov    0x164(%esp),%edx
 804a6ac:	89 54 24 04          	mov    %edx,0x4(%esp)
 804a6b0:	8b 84 24 60 01 00 00 	mov    0x160(%esp),%eax
 804a6b7:	89 04 24             	mov    %eax,(%esp)
 804a6ba:	e8 2d 4c 00 00       	call   804f2ec <__sprint_r>
 804a6bf:	85 c0                	test   %eax,%eax
 804a6c1:	0f 85 09 ec ff ff    	jne    80492d0 <_vfprintf_r+0x664>
 804a6c7:	8b b4 24 b0 00 00 00 	mov    0xb0(%esp),%esi
 804a6ce:	8d bc 24 00 01 00 00 	lea    0x100(%esp),%edi
 804a6d5:	e9 97 fe ff ff       	jmp    804a571 <_vfprintf_r+0x1905>
			if (ch >= 'f') {	/* 'f' or 'g' */
				if (_fpvalue == 0) {
					/* kludge for __dtoa irregularity */
					PRINT ("0", 1);
					if (expt < ndig || flags & ALT) {
						PRINT (decimal_point, decp_len);
 804a6da:	8d 84 24 cc 00 00 00 	lea    0xcc(%esp),%eax
 804a6e1:	89 44 24 08          	mov    %eax,0x8(%esp)
 804a6e5:	8b 94 24 64 01 00 00 	mov    0x164(%esp),%edx
 804a6ec:	89 54 24 04          	mov    %edx,0x4(%esp)
 804a6f0:	8b 84 24 60 01 00 00 	mov    0x160(%esp),%eax
 804a6f7:	89 04 24             	mov    %eax,(%esp)
 804a6fa:	e8 ed 4b 00 00       	call   804f2ec <__sprint_r>
 804a6ff:	85 c0                	test   %eax,%eax
 804a701:	0f 85 c9 eb ff ff    	jne    80492d0 <_vfprintf_r+0x664>
 804a707:	8d bc 24 00 01 00 00 	lea    0x100(%esp),%edi
 804a70e:	e9 5f f2 ff ff       	jmp    8049972 <_vfprintf_r+0xd06>
 804a713:	8b 44 24 44          	mov    0x44(%esp),%eax
 804a717:	2b 44 24 60          	sub    0x60(%esp),%eax
 804a71b:	89 44 24 50          	mov    %eax,0x50(%esp)
 804a71f:	e9 4c ed ff ff       	jmp    8049470 <_vfprintf_r+0x804>
					if (size > prec)
						size = prec;
				} else
					size = prec;
			} else
				size = strlen (cp);
 804a724:	8b 44 24 60          	mov    0x60(%esp),%eax
 804a728:	89 04 24             	mov    %eax,(%esp)
 804a72b:	89 54 24 20          	mov    %edx,0x20(%esp)
 804a72f:	e8 bc e4 ff ff       	call   8048bf0 <strlen>
 804a734:	89 44 24 50          	mov    %eax,0x50(%esp)
 804a738:	89 44 24 4c          	mov    %eax,0x4c(%esp)
 804a73c:	85 c0                	test   %eax,%eax
 804a73e:	8b 54 24 20          	mov    0x20(%esp),%edx
 804a742:	0f 89 6c ec ff ff    	jns    80493b4 <_vfprintf_r+0x748>
 804a748:	c7 44 24 4c 00 00 00 	movl   $0x0,0x4c(%esp)
 804a74f:	00 
 804a750:	e9 5f ec ff ff       	jmp    80493b4 <_vfprintf_r+0x748>
					}
				} else if (expt <= 0) {
					PRINT ("0", 1);
					if (expt || ndig || flags & ALT) {
						PRINT (decimal_point, decp_len);
						PAD (-expt, zeroes);
 804a755:	8d 84 24 cc 00 00 00 	lea    0xcc(%esp),%eax
 804a75c:	89 44 24 08          	mov    %eax,0x8(%esp)
 804a760:	8b 94 24 64 01 00 00 	mov    0x164(%esp),%edx
 804a767:	89 54 24 04          	mov    %edx,0x4(%esp)
 804a76b:	8b 84 24 60 01 00 00 	mov    0x160(%esp),%eax
 804a772:	89 04 24             	mov    %eax,(%esp)
 804a775:	e8 72 4b 00 00       	call   804f2ec <__sprint_r>
 804a77a:	85 c0                	test   %eax,%eax
 804a77c:	0f 85 4e eb ff ff    	jne    80492d0 <_vfprintf_r+0x664>
 804a782:	8d bc 24 00 01 00 00 	lea    0x100(%esp),%edi
 804a789:	8b 94 24 d0 00 00 00 	mov    0xd0(%esp),%edx
						PRINT (cp, ndig);
 804a790:	8b 44 24 60          	mov    0x60(%esp),%eax
 804a794:	89 07                	mov    %eax,(%edi)
 804a796:	8b 4c 24 70          	mov    0x70(%esp),%ecx
 804a79a:	e9 fd f3 ff ff       	jmp    8049b9c <_vfprintf_r+0xf30>
					}
				} else {
					char *convbuf = cp;
					PRINTANDPAD(cp, convbuf + ndig,
 804a79f:	8d 84 24 cc 00 00 00 	lea    0xcc(%esp),%eax
 804a7a6:	89 44 24 08          	mov    %eax,0x8(%esp)
 804a7aa:	8b 94 24 64 01 00 00 	mov    0x164(%esp),%edx
 804a7b1:	89 54 24 04          	mov    %edx,0x4(%esp)
 804a7b5:	8b 84 24 60 01 00 00 	mov    0x160(%esp),%eax
 804a7bc:	89 04 24             	mov    %eax,(%esp)
 804a7bf:	e8 28 4b 00 00       	call   804f2ec <__sprint_r>
 804a7c4:	85 c0                	test   %eax,%eax
 804a7c6:	0f 85 04 eb ff ff    	jne    80492d0 <_vfprintf_r+0x664>
 804a7cc:	8d bc 24 00 01 00 00 	lea    0x100(%esp),%edi
 804a7d3:	e9 f8 f5 ff ff       	jmp    8049dd0 <_vfprintf_r+0x1164>
 804a7d8:	31 f6                	xor    %esi,%esi
 804a7da:	e9 f9 f5 ff ff       	jmp    8049dd8 <_vfprintf_r+0x116c>
 804a7df:	8b 5c 24 50          	mov    0x50(%esp),%ebx
 804a7e3:	89 fe                	mov    %edi,%esi
 804a7e5:	89 c7                	mov    %eax,%edi
					}
				} else if (expt <= 0) {
					PRINT ("0", 1);
					if (expt || ndig || flags & ALT) {
						PRINT (decimal_point, decp_len);
						PAD (-expt, zeroes);
 804a7e7:	c7 07 44 18 05 08    	movl   $0x8051844,(%edi)
 804a7ed:	89 77 04             	mov    %esi,0x4(%edi)
 804a7f0:	01 b4 24 d4 00 00 00 	add    %esi,0xd4(%esp)
 804a7f7:	42                   	inc    %edx
 804a7f8:	89 94 24 d0 00 00 00 	mov    %edx,0xd0(%esp)
 804a7ff:	83 fa 07             	cmp    $0x7,%edx
 804a802:	0f 8f 4d ff ff ff    	jg     804a755 <_vfprintf_r+0x1ae9>
 804a808:	83 c7 08             	add    $0x8,%edi
 804a80b:	eb 83                	jmp    804a790 <_vfprintf_r+0x1b24>
					    if (cp > convbuf + ndig)
						cp = convbuf + ndig;
					}
#endif
					if (expt < ndig || flags & ALT)
					    PRINT (decimal_point, decp_len);
 804a80d:	8d 84 24 cc 00 00 00 	lea    0xcc(%esp),%eax
 804a814:	89 44 24 08          	mov    %eax,0x8(%esp)
 804a818:	8b 94 24 64 01 00 00 	mov    0x164(%esp),%edx
 804a81f:	89 54 24 04          	mov    %edx,0x4(%esp)
 804a823:	8b 84 24 60 01 00 00 	mov    0x160(%esp),%eax
 804a82a:	89 04 24             	mov    %eax,(%esp)
 804a82d:	e8 ba 4a 00 00       	call   804f2ec <__sprint_r>
 804a832:	85 c0                	test   %eax,%eax
 804a834:	0f 85 96 ea ff ff    	jne    80492d0 <_vfprintf_r+0x664>
 804a83a:	8b 84 24 b0 00 00 00 	mov    0xb0(%esp),%eax
 804a841:	8d bc 24 00 01 00 00 	lea    0x100(%esp),%edi
 804a848:	e9 12 f8 ff ff       	jmp    804a05f <_vfprintf_r+0x13f3>
					PRINTANDPAD (cp, convbuf + ndig,
 804a84d:	31 f6                	xor    %esi,%esi
 804a84f:	e9 54 f8 ff ff       	jmp    804a0a8 <_vfprintf_r+0x143c>
 804a854:	8d 84 24 cc 00 00 00 	lea    0xcc(%esp),%eax
 804a85b:	89 44 24 08          	mov    %eax,0x8(%esp)
 804a85f:	8b 94 24 64 01 00 00 	mov    0x164(%esp),%edx
 804a866:	89 54 24 04          	mov    %edx,0x4(%esp)
 804a86a:	8b 84 24 60 01 00 00 	mov    0x160(%esp),%eax
 804a871:	89 04 24             	mov    %eax,(%esp)
 804a874:	e8 73 4a 00 00       	call   804f2ec <__sprint_r>
 804a879:	85 c0                	test   %eax,%eax
 804a87b:	0f 85 4f ea ff ff    	jne    80492d0 <_vfprintf_r+0x664>
 804a881:	8b 44 24 70          	mov    0x70(%esp),%eax
 804a885:	2b 84 24 b0 00 00 00 	sub    0xb0(%esp),%eax
 804a88c:	8d bc 24 00 01 00 00 	lea    0x100(%esp),%edi
 804a893:	e9 08 f8 ff ff       	jmp    804a0a0 <_vfprintf_r+0x1434>
			   NULL string when precision is not 0.
			   However, if we are not optimizing for size,
			   we might as well mirror glibc behavior.  */
			if (cp == NULL) {
				cp = "(null)";
				size = ((unsigned) prec > 6U) ? 6 : prec;
 804a898:	89 6c 24 50          	mov    %ebp,0x50(%esp)
 804a89c:	83 fd 06             	cmp    $0x6,%ebp
 804a89f:	76 08                	jbe    804a8a9 <_vfprintf_r+0x1c3d>
 804a8a1:	c7 44 24 50 06 00 00 	movl   $0x6,0x50(%esp)
 804a8a8:	00 
 804a8a9:	8b 44 24 50          	mov    0x50(%esp),%eax
 804a8ad:	89 44 24 4c          	mov    %eax,0x4c(%esp)
 804a8b1:	85 c0                	test   %eax,%eax
 804a8b3:	0f 88 d5 02 00 00    	js     804ab8e <_vfprintf_r+0x1f22>
			goto nosign;
		case 's':
#ifdef _WANT_IO_C99_FORMATS
		case 'S':
#endif
			cp = GET_ARG (N, ap, char_ptr_t);
 804a8b9:	89 94 24 6c 01 00 00 	mov    %edx,0x16c(%esp)
			   NULL string when precision is not 0.
			   However, if we are not optimizing for size,
			   we might as well mirror glibc behavior.  */
			if (cp == NULL) {
				cp = "(null)";
				size = ((unsigned) prec > 6U) ? 6 : prec;
 804a8c0:	31 f6                	xor    %esi,%esi
#endif
		fmt_anchor = fmt;
		fmt++;		/* skip over '%' */

		flags = 0;
		dprec = 0;
 804a8c2:	c7 44 24 6c 00 00 00 	movl   $0x0,0x6c(%esp)
 804a8c9:	00 
		width = 0;
		prec = -1;
		sign = '\0';
#ifdef FLOATING_POINT
		lead = 0;
 804a8ca:	c7 44 24 78 00 00 00 	movl   $0x0,0x78(%esp)
 804a8d1:	00 
			/* Behavior is undefined if the user passed a
			   NULL string when precision is not 0.
			   However, if we are not optimizing for size,
			   we might as well mirror glibc behavior.  */
			if (cp == NULL) {
				cp = "(null)";
 804a8d2:	c7 44 24 60 c6 16 05 	movl   $0x80516c6,0x60(%esp)
 804a8d9:	08 
 804a8da:	e9 6d e5 ff ff       	jmp    8048e4c <_vfprintf_r+0x1e0>
# endif
					lead = expt;
			}

			if (softsign)
				sign = '-';
 804a8df:	c6 84 24 ad 00 00 00 	movb   $0x2d,0xad(%esp)
 804a8e6:	2d 
				prec = DEFPREC;
			} else if ((ch == 'g' || ch == 'G') && prec == 0) {
				prec = 1;
			}

			flags |= FPT;
 804a8e7:	8b 4c 24 58          	mov    0x58(%esp),%ecx
 804a8eb:	89 4c 24 48          	mov    %ecx,0x48(%esp)
# endif
					lead = expt;
			}

			if (softsign)
				sign = '-';
 804a8ef:	be 2d 00 00 00       	mov    $0x2d,%esi
#endif
		fmt_anchor = fmt;
		fmt++;		/* skip over '%' */

		flags = 0;
		dprec = 0;
 804a8f4:	c7 44 24 6c 00 00 00 	movl   $0x0,0x6c(%esp)
 804a8fb:	00 
 804a8fc:	e9 95 eb ff ff       	jmp    8049496 <_vfprintf_r+0x82a>
			}
			if (isnan (_fpvalue)) {
				if (ch <= 'G') /* 'A', 'E', 'F', or 'G' */
					cp = "NAN";
				else
					cp = "nan";
 804a901:	c7 44 24 60 a0 16 05 	movl   $0x80516a0,0x60(%esp)
 804a908:	08 
 804a909:	e9 4a fd ff ff       	jmp    804a658 <_vfprintf_r+0x19ec>
			   zeros are not permitted.  Otherwise, scanf
			   could not read what printf wrote.
			 */
			if (isinf (_fpvalue)) {
				if (_fpvalue < 0)
					sign = '-';
 804a90e:	c6 84 24 ad 00 00 00 	movb   $0x2d,0xad(%esp)
 804a915:	2d 
 804a916:	be 2d 00 00 00       	mov    $0x2d,%esi
 804a91b:	e9 2d ed ff ff       	jmp    804964d <_vfprintf_r+0x9e1>

			cp = cvt (data, _fpvalue, prec, flags, &softsign,
				  &expt, ch, &ndig, cp);

			if (ch == 'g' || ch == 'G') {
				if (expt <= -4 || expt > prec)
 804a920:	8b 84 24 b0 00 00 00 	mov    0xb0(%esp),%eax
 804a927:	89 c1                	mov    %eax,%ecx
 804a929:	83 f8 fd             	cmp    $0xfffffffd,%eax
 804a92c:	7c 08                	jl     804a936 <_vfprintf_r+0x1cca>
 804a92e:	39 c5                	cmp    %eax,%ebp
 804a930:	0f 8d 90 00 00 00    	jge    804a9c6 <_vfprintf_r+0x1d5a>
					ch -= 2; /* 'e' or 'E' */
 804a936:	83 6c 24 68 02       	subl   $0x2,0x68(%esp)
# ifdef _WANT_IO_C99_FORMATS
			else if (ch == 'F')
				ch = 'f';
# endif
			if (ch <= 'e') {	/* 'a', 'A', 'e', or 'E' fmt */
				--expt;
 804a93b:	49                   	dec    %ecx
 804a93c:	89 8c 24 b0 00 00 00 	mov    %ecx,0xb0(%esp)
# else
#  define isa 0
# endif

	p = p0;
	*p++ = isa ? 'p' - 'a' + fmtch : fmtch;
 804a943:	8a 54 24 68          	mov    0x68(%esp),%dl
 804a947:	88 94 24 be 00 00 00 	mov    %dl,0xbe(%esp)
	if (exp < 0) {
 804a94e:	85 c9                	test   %ecx,%ecx
 804a950:	0f 88 d2 02 00 00    	js     804ac28 <_vfprintf_r+0x1fbc>
		exp = -exp;
		*p++ = '-';
	}
	else
		*p++ = '+';
 804a956:	c6 84 24 bf 00 00 00 	movb   $0x2b,0xbf(%esp)
 804a95d:	2b 
	t = expbuf + MAXEXPLEN;
	if (exp > 9) {
 804a95e:	83 f9 09             	cmp    $0x9,%ecx
 804a961:	0f 8f a3 01 00 00    	jg     804ab0a <_vfprintf_r+0x1e9e>
		*--t = to_char (exp);
		for (; t < expbuf + MAXEXPLEN; *p++ = *t++);
	}
	else {
		if (!isa)
			*p++ = '0';
 804a967:	c6 84 24 c0 00 00 00 	movb   $0x30,0xc0(%esp)
 804a96e:	30 
		*p++ = to_char (exp);
 804a96f:	83 c1 30             	add    $0x30,%ecx
 804a972:	88 8c 24 c1 00 00 00 	mov    %cl,0xc1(%esp)
 804a979:	8d 84 24 c2 00 00 00 	lea    0xc2(%esp),%eax
			else if (ch == 'F')
				ch = 'f';
# endif
			if (ch <= 'e') {	/* 'a', 'A', 'e', or 'E' fmt */
				--expt;
				expsize = exponent (expstr, expt, ch);
 804a980:	8d 94 24 be 00 00 00 	lea    0xbe(%esp),%edx
	else {
		if (!isa)
			*p++ = '0';
		*p++ = to_char (exp);
	}
	return (p - p0);
 804a987:	29 d0                	sub    %edx,%eax
 804a989:	89 84 24 88 00 00 00 	mov    %eax,0x88(%esp)
				ch = 'f';
# endif
			if (ch <= 'e') {	/* 'a', 'A', 'e', or 'E' fmt */
				--expt;
				expsize = exponent (expstr, expt, ch);
				size = expsize + ndig;
 804a990:	8b 54 24 70          	mov    0x70(%esp),%edx
 804a994:	01 c2                	add    %eax,%edx
 804a996:	89 54 24 50          	mov    %edx,0x50(%esp)
				if (ndig > 1 || flags & ALT)
 804a99a:	83 7c 24 70 01       	cmpl   $0x1,0x70(%esp)
 804a99f:	0f 8e e3 02 00 00    	jle    804ac88 <_vfprintf_r+0x201c>
					++size;
 804a9a5:	ff 44 24 50          	incl   0x50(%esp)
 804a9a9:	8b 44 24 50          	mov    0x50(%esp),%eax
 804a9ad:	89 44 24 4c          	mov    %eax,0x4c(%esp)
 804a9b1:	85 c0                	test   %eax,%eax
 804a9b3:	0f 88 e6 02 00 00    	js     804ac9f <_vfprintf_r+0x2033>
		dprec = 0;
		width = 0;
		prec = -1;
		sign = '\0';
#ifdef FLOATING_POINT
		lead = 0;
 804a9b9:	c7 44 24 78 00 00 00 	movl   $0x0,0x78(%esp)
 804a9c0:	00 
 804a9c1:	e9 a0 fa ff ff       	jmp    804a466 <_vfprintf_r+0x17fa>
							size += prec + 1;
					} else	/* "0.X" */
						size = (prec || flags & ALT)
							  ? prec + 2
							  : 1;
				} else if (expt >= ndig) { /* fixed g fmt */
 804a9c6:	3b 44 24 70          	cmp    0x70(%esp),%eax
 804a9ca:	0f 8c 0c 01 00 00    	jl     804aadc <_vfprintf_r+0x1e70>
					size = expt;
					if (flags & ALT)
 804a9d0:	f6 44 24 48 01       	testb  $0x1,0x48(%esp)
 804a9d5:	0f 85 0c 02 00 00    	jne    804abe7 <_vfprintf_r+0x1f7b>
 804a9db:	89 44 24 4c          	mov    %eax,0x4c(%esp)
 804a9df:	85 c0                	test   %eax,%eax
 804a9e1:	0f 88 c5 02 00 00    	js     804acac <_vfprintf_r+0x2040>
 804a9e7:	89 4c 24 50          	mov    %ecx,0x50(%esp)
 804a9eb:	c7 44 24 68 67 00 00 	movl   $0x67,0x68(%esp)
 804a9f2:	00 
 804a9f3:	e9 6a fa ff ff       	jmp    804a462 <_vfprintf_r+0x17f6>
			ndigits++;
		}
		mode = 2;		/* ndigits significant digits */
	}

	digits = _DTOA_R (data, value, mode, ndigits, decpt, &dsgn, &rve);
 804a9f8:	8d 84 24 b8 00 00 00 	lea    0xb8(%esp),%eax
 804a9ff:	89 44 24 1c          	mov    %eax,0x1c(%esp)
 804aa03:	8d 84 24 b4 00 00 00 	lea    0xb4(%esp),%eax
 804aa0a:	89 44 24 18          	mov    %eax,0x18(%esp)
				prec = 1;
			}

			flags |= FPT;

			cp = cvt (data, _fpvalue, prec, flags, &softsign,
 804aa0e:	8d 84 24 b0 00 00 00 	lea    0xb0(%esp),%eax
 804aa15:	89 44 24 14          	mov    %eax,0x14(%esp)
			ndigits++;
		}
		mode = 2;		/* ndigits significant digits */
	}

	digits = _DTOA_R (data, value, mode, ndigits, decpt, &dsgn, &rve);
 804aa19:	89 6c 24 10          	mov    %ebp,0x10(%esp)
 804aa1d:	c7 44 24 0c 03 00 00 	movl   $0x3,0xc(%esp)
 804aa24:	00 
 804aa25:	dd 54 24 04          	fstl   0x4(%esp)
 804aa29:	8b 84 24 60 01 00 00 	mov    0x160(%esp),%eax
 804aa30:	89 04 24             	mov    %eax,(%esp)
 804aa33:	dd 5c 24 20          	fstpl  0x20(%esp)
 804aa37:	e8 bc 07 00 00       	call   804b1f8 <_dtoa_r>
 804aa3c:	89 44 24 60          	mov    %eax,0x60(%esp)

	if ((ch != 'g' && ch != 'G') || flags & ALT) {	/* Print trailing zeros */
		bp = digits + ndigits;
 804aa40:	89 c2                	mov    %eax,%edx
 804aa42:	01 ea                	add    %ebp,%edx
 804aa44:	89 ee                	mov    %ebp,%esi
 804aa46:	dd 44 24 20          	fldl   0x20(%esp)
		if (ch == 'f' || ch == 'F') {
			if (*digits == '0' && value)
 804aa4a:	8b 44 24 60          	mov    0x60(%esp),%eax
 804aa4e:	80 38 30             	cmpb   $0x30,(%eax)
 804aa51:	0f 84 65 01 00 00    	je     804abbc <_vfprintf_r+0x1f50>
 804aa57:	8b 84 24 b0 00 00 00 	mov    0xb0(%esp),%eax
				*decpt = -ndigits + 1;
			bp += *decpt;
 804aa5e:	01 c2                	add    %eax,%edx
 804aa60:	e9 7a f9 ff ff       	jmp    804a3df <_vfprintf_r+0x1773>
				  cp = buf;
			} else
# endif /* _WANT_IO_C99_FORMATS */
			if (prec == -1) {
				prec = DEFPREC;
			} else if ((ch == 'g' || ch == 'G') && prec == 0) {
 804aa65:	85 ed                	test   %ebp,%ebp
 804aa67:	0f 85 a8 f8 ff ff    	jne    804a315 <_vfprintf_r+0x16a9>
				prec = 1;
 804aa6d:	bd 01 00 00 00       	mov    $0x1,%ebp
 804aa72:	e9 9e f8 ff ff       	jmp    804a315 <_vfprintf_r+0x16a9>
						PAD (ndig - 1, zeroes);
					}
				} else if (expt <= 0) {
					PRINT ("0", 1);
					if (expt || ndig || flags & ALT) {
						PRINT (decimal_point, decp_len);
 804aa77:	8d 84 24 cc 00 00 00 	lea    0xcc(%esp),%eax
 804aa7e:	89 44 24 08          	mov    %eax,0x8(%esp)
 804aa82:	8b 94 24 64 01 00 00 	mov    0x164(%esp),%edx
 804aa89:	89 54 24 04          	mov    %edx,0x4(%esp)
 804aa8d:	8b 84 24 60 01 00 00 	mov    0x160(%esp),%eax
 804aa94:	89 04 24             	mov    %eax,(%esp)
 804aa97:	e8 50 48 00 00       	call   804f2ec <__sprint_r>
 804aa9c:	85 c0                	test   %eax,%eax
 804aa9e:	0f 85 2c e8 ff ff    	jne    80492d0 <_vfprintf_r+0x664>
 804aaa4:	8b b4 24 b0 00 00 00 	mov    0xb0(%esp),%esi
 804aaab:	8d bc 24 00 01 00 00 	lea    0x100(%esp),%edi
 804aab2:	8b 94 24 d0 00 00 00 	mov    0xd0(%esp),%edx
 804aab9:	e9 f9 fa ff ff       	jmp    804a5b7 <_vfprintf_r+0x194b>
				else
				  cp = buf;
			} else
# endif /* _WANT_IO_C99_FORMATS */
			if (prec == -1) {
				prec = DEFPREC;
 804aabe:	bd 06 00 00 00       	mov    $0x6,%ebp
 804aac3:	e9 4d f8 ff ff       	jmp    804a315 <_vfprintf_r+0x16a9>
# ifdef _NO_LONGDBL
	union double_union tmp;

	tmp.d = value;
	if (word0 (tmp) & Sign_bit) { /* this will check for < 0 and -0.0 */
		value = -value;
 804aac8:	d9 e0                	fchs   
		*sign = '-';
 804aaca:	c6 44 24 64 2d       	movb   $0x2d,0x64(%esp)
 804aacf:	e9 67 f8 ff ff       	jmp    804a33b <_vfprintf_r+0x16cf>
		/* To obtain ndigits after the decimal point for the 'e'
		 * and 'E' formats, round to ndigits + 1 significant
		 * figures.
		 */
		if (ch == 'e' || ch == 'E') {
			ndigits++;
 804aad4:	8d 75 01             	lea    0x1(%ebp),%esi
 804aad7:	e9 8d f8 ff ff       	jmp    804a369 <_vfprintf_r+0x16fd>
					size = expt;
					if (flags & ALT)
						++size;
				} else
					size = ndig + (expt > 0 ?
						1 : 2 - expt);
 804aadc:	85 c0                	test   %eax,%eax
 804aade:	0f 8e 53 01 00 00    	jle    804ac37 <_vfprintf_r+0x1fcb>
 804aae4:	ba 01 00 00 00       	mov    $0x1,%edx
				} else if (expt >= ndig) { /* fixed g fmt */
					size = expt;
					if (flags & ALT)
						++size;
				} else
					size = ndig + (expt > 0 ?
 804aae9:	03 54 24 70          	add    0x70(%esp),%edx
 804aaed:	89 54 24 50          	mov    %edx,0x50(%esp)
 804aaf1:	89 54 24 4c          	mov    %edx,0x4c(%esp)
 804aaf5:	85 d2                	test   %edx,%edx
 804aaf7:	0f 88 fd 00 00 00    	js     804abfa <_vfprintf_r+0x1f8e>
 804aafd:	c7 44 24 68 67 00 00 	movl   $0x67,0x68(%esp)
 804ab04:	00 
 804ab05:	e9 58 f9 ff ff       	jmp    804a462 <_vfprintf_r+0x17f6>
		exp = -exp;
		*p++ = '-';
	}
	else
		*p++ = '+';
	t = expbuf + MAXEXPLEN;
 804ab0a:	8d ac 24 cc 00 00 00 	lea    0xcc(%esp),%ebp
 804ab11:	89 ee                	mov    %ebp,%esi
	if (exp > 9) {
		do {
			*--t = to_char (exp % 10);
		} while ((exp /= 10) > 9);
 804ab13:	89 5c 24 50          	mov    %ebx,0x50(%esp)
	else
		*p++ = '+';
	t = expbuf + MAXEXPLEN;
	if (exp > 9) {
		do {
			*--t = to_char (exp % 10);
 804ab17:	4e                   	dec    %esi
 804ab18:	89 c8                	mov    %ecx,%eax
 804ab1a:	bb 0a 00 00 00       	mov    $0xa,%ebx
 804ab1f:	99                   	cltd   
 804ab20:	f7 fb                	idiv   %ebx
 804ab22:	89 54 24 4c          	mov    %edx,0x4c(%esp)
 804ab26:	8a 54 24 4c          	mov    0x4c(%esp),%dl
 804ab2a:	83 c2 30             	add    $0x30,%edx
 804ab2d:	88 16                	mov    %dl,(%esi)
		} while ((exp /= 10) > 9);
 804ab2f:	b8 67 66 66 66       	mov    $0x66666667,%eax
 804ab34:	f7 e9                	imul   %ecx
 804ab36:	c1 fa 02             	sar    $0x2,%edx
 804ab39:	c1 f9 1f             	sar    $0x1f,%ecx
 804ab3c:	29 ca                	sub    %ecx,%edx
 804ab3e:	89 d1                	mov    %edx,%ecx
 804ab40:	83 fa 09             	cmp    $0x9,%edx
 804ab43:	7f d2                	jg     804ab17 <_vfprintf_r+0x1eab>
 804ab45:	8b 5c 24 50          	mov    0x50(%esp),%ebx
		*--t = to_char (exp);
 804ab49:	8d 46 ff             	lea    -0x1(%esi),%eax
 804ab4c:	83 c1 30             	add    $0x30,%ecx
 804ab4f:	88 4e ff             	mov    %cl,-0x1(%esi)
		for (; t < expbuf + MAXEXPLEN; *p++ = *t++);
 804ab52:	39 e8                	cmp    %ebp,%eax
 804ab54:	0f 83 5f 01 00 00    	jae    804acb9 <_vfprintf_r+0x204d>
 804ab5a:	8d 94 24 c0 00 00 00 	lea    0xc0(%esp),%edx
 804ab61:	eb 03                	jmp    804ab66 <_vfprintf_r+0x1efa>
 804ab63:	90                   	nop
 804ab64:	8a 08                	mov    (%eax),%cl
 804ab66:	88 0a                	mov    %cl,(%edx)
 804ab68:	42                   	inc    %edx
 804ab69:	40                   	inc    %eax
 804ab6a:	39 e8                	cmp    %ebp,%eax
 804ab6c:	75 f6                	jne    804ab64 <_vfprintf_r+0x1ef8>
               _ATTRIBUTE ((__format__ (__printf__, 3, 0))));
int	_EXFUN(_vfiprintf_r, (struct _reent *, FILE *, const char *, __VALIST)
               _ATTRIBUTE ((__format__ (__printf__, 3, 0))));
int	_EXFUN(_vfiscanf_r, (struct _reent *, FILE *, const char *, __VALIST)
               _ATTRIBUTE ((__format__ (__scanf__, 3, 0))));
int	_EXFUN(_vfprintf_r, (struct _reent *, FILE *__restrict, const char *__restrict, __VALIST)
 804ab6e:	8d 84 24 40 01 00 00 	lea    0x140(%esp),%eax
 804ab75:	29 f0                	sub    %esi,%eax
 804ab77:	8d 44 04 4d          	lea    0x4d(%esp,%eax,1),%eax
 804ab7b:	e9 00 fe ff ff       	jmp    804a980 <_vfprintf_r+0x1d14>
 804ab80:	dd d8                	fstp   %st(0)
 804ab82:	8b 94 24 b8 00 00 00 	mov    0xb8(%esp),%edx
 804ab89:	e9 81 f8 ff ff       	jmp    804a40f <_vfprintf_r+0x17a3>
 804ab8e:	c7 44 24 4c 00 00 00 	movl   $0x0,0x4c(%esp)
 804ab95:	00 
 804ab96:	e9 1e fd ff ff       	jmp    804a8b9 <_vfprintf_r+0x1c4d>
			} else {
				if (ch == 'f') {		/* f fmt */
					if (expt > 0) {
						size = expt;
						if (prec || flags & ALT)
							size += prec + 1;
 804ab9b:	8d 6c 05 01          	lea    0x1(%ebp,%eax,1),%ebp
 804ab9f:	89 6c 24 50          	mov    %ebp,0x50(%esp)
 804aba3:	89 6c 24 4c          	mov    %ebp,0x4c(%esp)
 804aba7:	85 ed                	test   %ebp,%ebp
 804aba9:	0f 89 b3 f8 ff ff    	jns    804a462 <_vfprintf_r+0x17f6>
 804abaf:	c7 44 24 4c 00 00 00 	movl   $0x0,0x4c(%esp)
 804abb6:	00 
					} else	/* "0.X" */
						size = (prec || flags & ALT)
							  ? prec + 2
							  : 1;
 804abb7:	e9 a6 f8 ff ff       	jmp    804a462 <_vfprintf_r+0x17f6>
	digits = _DTOA_R (data, value, mode, ndigits, decpt, &dsgn, &rve);

	if ((ch != 'g' && ch != 'G') || flags & ALT) {	/* Print trailing zeros */
		bp = digits + ndigits;
		if (ch == 'f' || ch == 'F') {
			if (*digits == '0' && value)
 804abbc:	d9 ee                	fldz   
 804abbe:	d9 c9                	fxch   %st(1)
 804abc0:	dd e1                	fucom  %st(1)
 804abc2:	df e0                	fnstsw %ax
 804abc4:	dd d9                	fstp   %st(1)
 804abc6:	80 e4 45             	and    $0x45,%ah
 804abc9:	80 f4 40             	xor    $0x40,%ah
 804abcc:	0f 84 85 fe ff ff    	je     804aa57 <_vfprintf_r+0x1deb>
				*decpt = -ndigits + 1;
 804abd2:	b8 01 00 00 00       	mov    $0x1,%eax
 804abd7:	29 f0                	sub    %esi,%eax
 804abd9:	89 84 24 b0 00 00 00 	mov    %eax,0xb0(%esp)
			bp += *decpt;
 804abe0:	01 c2                	add    %eax,%edx
 804abe2:	e9 f8 f7 ff ff       	jmp    804a3df <_vfprintf_r+0x1773>
							  ? prec + 2
							  : 1;
				} else if (expt >= ndig) { /* fixed g fmt */
					size = expt;
					if (flags & ALT)
						++size;
 804abe7:	8d 50 01             	lea    0x1(%eax),%edx
 804abea:	89 54 24 50          	mov    %edx,0x50(%esp)
 804abee:	89 54 24 4c          	mov    %edx,0x4c(%esp)
 804abf2:	85 d2                	test   %edx,%edx
 804abf4:	0f 89 03 ff ff ff    	jns    804aafd <_vfprintf_r+0x1e91>
 804abfa:	c7 44 24 4c 00 00 00 	movl   $0x0,0x4c(%esp)
 804ac01:	00 
 804ac02:	e9 f6 fe ff ff       	jmp    804aafd <_vfprintf_r+0x1e91>
						if (prec || flags & ALT)
							size += prec + 1;
					} else	/* "0.X" */
						size = (prec || flags & ALT)
							  ? prec + 2
							  : 1;
 804ac07:	85 ed                	test   %ebp,%ebp
 804ac09:	75 38                	jne    804ac43 <_vfprintf_r+0x1fd7>
					if (expt > 0) {
						size = expt;
						if (prec || flags & ALT)
							size += prec + 1;
					} else	/* "0.X" */
						size = (prec || flags & ALT)
 804ac0b:	f6 44 24 48 01       	testb  $0x1,0x48(%esp)
 804ac10:	75 31                	jne    804ac43 <_vfprintf_r+0x1fd7>
 804ac12:	c7 44 24 4c 01 00 00 	movl   $0x1,0x4c(%esp)
 804ac19:	00 
							  ? prec + 2
							  : 1;
 804ac1a:	c7 44 24 50 01 00 00 	movl   $0x1,0x50(%esp)
 804ac21:	00 
 804ac22:	e9 3b f8 ff ff       	jmp    804a462 <_vfprintf_r+0x17f6>
 804ac27:	90                   	nop
# endif

	p = p0;
	*p++ = isa ? 'p' - 'a' + fmtch : fmtch;
	if (exp < 0) {
		exp = -exp;
 804ac28:	f7 d9                	neg    %ecx
		*p++ = '-';
 804ac2a:	c6 84 24 bf 00 00 00 	movb   $0x2d,0xbf(%esp)
 804ac31:	2d 
 804ac32:	e9 27 fd ff ff       	jmp    804a95e <_vfprintf_r+0x1cf2>
					size = expt;
					if (flags & ALT)
						++size;
				} else
					size = ndig + (expt > 0 ?
						1 : 2 - expt);
 804ac37:	ba 02 00 00 00       	mov    $0x2,%edx
 804ac3c:	29 c2                	sub    %eax,%edx
 804ac3e:	e9 a6 fe ff ff       	jmp    804aae9 <_vfprintf_r+0x1e7d>
						if (prec || flags & ALT)
							size += prec + 1;
					} else	/* "0.X" */
						size = (prec || flags & ALT)
							  ? prec + 2
							  : 1;
 804ac43:	83 c5 02             	add    $0x2,%ebp
 804ac46:	89 6c 24 50          	mov    %ebp,0x50(%esp)
 804ac4a:	89 6c 24 4c          	mov    %ebp,0x4c(%esp)
 804ac4e:	85 ed                	test   %ebp,%ebp
 804ac50:	0f 89 0c f8 ff ff    	jns    804a462 <_vfprintf_r+0x17f6>
 804ac56:	e9 54 ff ff ff       	jmp    804abaf <_vfprintf_r+0x1f43>
 804ac5b:	90                   	nop
						fmt = old_fmt;
						goto rflag;
					}
				}
#endif /* !_NO_POS_ARGS */
				prec = GET_ARG (n, ap, int);
 804ac5c:	8b 84 24 6c 01 00 00 	mov    0x16c(%esp),%eax
 804ac63:	83 c0 04             	add    $0x4,%eax
 804ac66:	8b 94 24 6c 01 00 00 	mov    0x16c(%esp),%edx
 804ac6d:	8b 2a                	mov    (%edx),%ebp
#ifndef _NO_POS_ARGS
				is_pos_arg = old_is_pos_arg;
#endif
				if (prec < 0)
 804ac6f:	85 ed                	test   %ebp,%ebp
 804ac71:	0f 89 60 e8 ff ff    	jns    80494d7 <_vfprintf_r+0x86b>
						fmt = old_fmt;
						goto rflag;
					}
				}
#endif /* !_NO_POS_ARGS */
				prec = GET_ARG (n, ap, int);
 804ac77:	89 84 24 6c 01 00 00 	mov    %eax,0x16c(%esp)
#ifndef _NO_POS_ARGS
				is_pos_arg = old_is_pos_arg;
#endif
				if (prec < 0)
					prec = -1;
 804ac7e:	bd ff ff ff ff       	mov    $0xffffffff,%ebp
 804ac83:	e9 65 e1 ff ff       	jmp    8048ded <_vfprintf_r+0x181>
# endif
			if (ch <= 'e') {	/* 'a', 'A', 'e', or 'E' fmt */
				--expt;
				expsize = exponent (expstr, expt, ch);
				size = expsize + ndig;
				if (ndig > 1 || flags & ALT)
 804ac88:	f6 44 24 48 01       	testb  $0x1,0x48(%esp)
 804ac8d:	0f 85 12 fd ff ff    	jne    804a9a5 <_vfprintf_r+0x1d39>
 804ac93:	89 54 24 4c          	mov    %edx,0x4c(%esp)
 804ac97:	85 d2                	test   %edx,%edx
 804ac99:	0f 89 1a fd ff ff    	jns    804a9b9 <_vfprintf_r+0x1d4d>
 804ac9f:	c7 44 24 4c 00 00 00 	movl   $0x0,0x4c(%esp)
 804aca6:	00 
 804aca7:	e9 0d fd ff ff       	jmp    804a9b9 <_vfprintf_r+0x1d4d>
 804acac:	c7 44 24 4c 00 00 00 	movl   $0x0,0x4c(%esp)
 804acb3:	00 
 804acb4:	e9 2e fd ff ff       	jmp    804a9e7 <_vfprintf_r+0x1d7b>
	if (exp > 9) {
		do {
			*--t = to_char (exp % 10);
		} while ((exp /= 10) > 9);
		*--t = to_char (exp);
		for (; t < expbuf + MAXEXPLEN; *p++ = *t++);
 804acb9:	8d 84 24 c0 00 00 00 	lea    0xc0(%esp),%eax
 804acc0:	e9 bb fc ff ff       	jmp    804a980 <_vfprintf_r+0x1d14>
				*decpt = -ndigits + 1;
			bp += *decpt;
		}
		if (value == 0)	/* kludge for __dtoa irregularity */
			rve = bp;
		while (rve < bp)
 804acc5:	89 c2                	mov    %eax,%edx
 804acc7:	e9 43 f7 ff ff       	jmp    804a40f <_vfprintf_r+0x17a3>
 804accc:	8b 8c 24 b0 00 00 00 	mov    0xb0(%esp),%ecx
 804acd3:	e9 63 fc ff ff       	jmp    804a93b <_vfprintf_r+0x1ccf>

0804acd8 <__sbprintf>:
_DEFUN(__sbprintf, (rptr, fp, fmt, ap),
       struct _reent *rptr _AND
       register FILE *fp   _AND
       _CONST char *fmt  _AND
       va_list ap)
{
 804acd8:	55                   	push   %ebp
 804acd9:	57                   	push   %edi
 804acda:	56                   	push   %esi
 804acdb:	53                   	push   %ebx
 804acdc:	81 ec 8c 04 00 00    	sub    $0x48c,%esp
 804ace2:	89 c7                	mov    %eax,%edi
 804ace4:	89 d3                	mov    %edx,%ebx
	int ret;
	FILE fake;
	unsigned char buf[BUFSIZ];

	/* copy the important variables */
	fake._flags = fp->_flags & ~__SNBF;
 804ace6:	8b 52 0c             	mov    0xc(%edx),%edx
 804ace9:	83 e2 fd             	and    $0xfffffffd,%edx
 804acec:	66 89 54 24 24       	mov    %dx,0x24(%esp)
	fake._flags2 = fp->_flags2;
 804acf1:	8b 43 64             	mov    0x64(%ebx),%eax
 804acf4:	89 44 24 7c          	mov    %eax,0x7c(%esp)
	fake._file = fp->_file;
 804acf8:	66 8b 43 0e          	mov    0xe(%ebx),%ax
 804acfc:	66 89 44 24 26       	mov    %ax,0x26(%esp)
	fake._cookie = fp->_cookie;
 804ad01:	8b 43 1c             	mov    0x1c(%ebx),%eax
 804ad04:	89 44 24 34          	mov    %eax,0x34(%esp)
	fake._write = fp->_write;
 804ad08:	8b 43 24             	mov    0x24(%ebx),%eax
 804ad0b:	89 44 24 3c          	mov    %eax,0x3c(%esp)

	/* set up the buffer */
	fake._bf._base = fake._p = buf;
 804ad0f:	8d 84 24 80 00 00 00 	lea    0x80(%esp),%eax
 804ad16:	89 44 24 18          	mov    %eax,0x18(%esp)
 804ad1a:	89 44 24 28          	mov    %eax,0x28(%esp)
	fake._bf._size = fake._w = sizeof (buf);
 804ad1e:	c7 44 24 20 00 04 00 	movl   $0x400,0x20(%esp)
 804ad25:	00 
 804ad26:	c7 44 24 2c 00 04 00 	movl   $0x400,0x2c(%esp)
 804ad2d:	00 
	fake._lbfsize = 0;	/* not actually used, but Just In Case */
 804ad2e:	c7 44 24 30 00 00 00 	movl   $0x0,0x30(%esp)
 804ad35:	00 
#ifndef __SINGLE_THREAD__
	__lock_init_recursive (fake._lock);
#endif

	/* do the work, then copy any error status */
	ret = _VFPRINTF_R (rptr, &fake, fmt, ap);
 804ad36:	8b 84 24 a0 04 00 00 	mov    0x4a0(%esp),%eax
 804ad3d:	89 44 24 0c          	mov    %eax,0xc(%esp)
 804ad41:	89 4c 24 08          	mov    %ecx,0x8(%esp)
 804ad45:	8d 6c 24 18          	lea    0x18(%esp),%ebp
 804ad49:	89 6c 24 04          	mov    %ebp,0x4(%esp)
 804ad4d:	89 3c 24             	mov    %edi,(%esp)
 804ad50:	e8 17 df ff ff       	call   8048c6c <_vfprintf_r>
 804ad55:	89 c6                	mov    %eax,%esi
	if (ret >= 0 && _fflush_r (rptr, &fake))
 804ad57:	85 c0                	test   %eax,%eax
 804ad59:	78 10                	js     804ad6b <__sbprintf+0x93>
 804ad5b:	89 6c 24 04          	mov    %ebp,0x4(%esp)
 804ad5f:	89 3c 24             	mov    %edi,(%esp)
 804ad62:	e8 59 19 00 00       	call   804c6c0 <_fflush_r>
 804ad67:	85 c0                	test   %eax,%eax
 804ad69:	75 19                	jne    804ad84 <__sbprintf+0xac>
		ret = EOF;
	if (fake._flags & __SERR)
 804ad6b:	f6 44 24 24 40       	testb  $0x40,0x24(%esp)
 804ad70:	74 05                	je     804ad77 <__sbprintf+0x9f>
		fp->_flags |= __SERR;
 804ad72:	66 83 4b 0c 40       	orw    $0x40,0xc(%ebx)

#ifndef __SINGLE_THREAD__
	__lock_close_recursive (fake._lock);
#endif
	return (ret);
}
 804ad77:	89 f0                	mov    %esi,%eax
 804ad79:	81 c4 8c 04 00 00    	add    $0x48c,%esp
 804ad7f:	5b                   	pop    %ebx
 804ad80:	5e                   	pop    %esi
 804ad81:	5f                   	pop    %edi
 804ad82:	5d                   	pop    %ebp
 804ad83:	c3                   	ret    
#endif

	/* do the work, then copy any error status */
	ret = _VFPRINTF_R (rptr, &fake, fmt, ap);
	if (ret >= 0 && _fflush_r (rptr, &fake))
		ret = EOF;
 804ad84:	be ff ff ff ff       	mov    $0xffffffff,%esi
 804ad89:	eb e0                	jmp    804ad6b <__sbprintf+0x93>
 804ad8b:	90                   	nop

0804ad8c <vfprintf>:
int
_DEFUN(VFPRINTF, (fp, fmt0, ap),
       FILE * fp         _AND
       _CONST char *fmt0 _AND
       va_list ap)
{
 804ad8c:	83 ec 1c             	sub    $0x1c,%esp
  int result;
  result = _VFPRINTF_R (_REENT, fp, fmt0, ap);
 804ad8f:	8b 44 24 28          	mov    0x28(%esp),%eax
 804ad93:	89 44 24 0c          	mov    %eax,0xc(%esp)
 804ad97:	8b 44 24 24          	mov    0x24(%esp),%eax
 804ad9b:	89 44 24 08          	mov    %eax,0x8(%esp)
 804ad9f:	8b 44 24 20          	mov    0x20(%esp),%eax
 804ada3:	89 44 24 04          	mov    %eax,0x4(%esp)
 804ada7:	a1 40 40 05 08       	mov    0x8054040,%eax
 804adac:	89 04 24             	mov    %eax,(%esp)
 804adaf:	e8 b8 de ff ff       	call   8048c6c <_vfprintf_r>
  return result;
}
 804adb4:	83 c4 1c             	add    $0x1c,%esp
 804adb7:	c3                   	ret    

0804adb8 <__swsetup_r>:

int
_DEFUN(__swsetup_r, (ptr, fp),
       struct _reent *ptr _AND
       register FILE * fp)
{
 804adb8:	53                   	push   %ebx
 804adb9:	83 ec 18             	sub    $0x18,%esp
 804adbc:	8b 5c 24 24          	mov    0x24(%esp),%ebx
  /* Make sure stdio is set up.  */

  CHECK_INIT (_REENT, fp);
 804adc0:	a1 40 40 05 08       	mov    0x8054040,%eax
 804adc5:	85 c0                	test   %eax,%eax
 804adc7:	74 0b                	je     804add4 <__swsetup_r+0x1c>
 804adc9:	8b 50 38             	mov    0x38(%eax),%edx
 804adcc:	85 d2                	test   %edx,%edx
 804adce:	0f 84 9c 00 00 00    	je     804ae70 <__swsetup_r+0xb8>

  /*
   * If we are not writing, we had better be reading and writing.
   */

  if ((fp->_flags & __SWR) == 0)
 804add4:	8b 43 0c             	mov    0xc(%ebx),%eax
 804add7:	a8 08                	test   $0x8,%al
 804add9:	74 25                	je     804ae00 <__swsetup_r+0x48>
 804addb:	8b 53 10             	mov    0x10(%ebx),%edx
  /*
   * Make a buffer if necessary, then set _w.
   * A string I/O file should not explicitly allocate a buffer
   * unless asprintf is being used.
   */
  if (fp->_bf._base == NULL
 804adde:	85 d2                	test   %edx,%edx
 804ade0:	74 38                	je     804ae1a <__swsetup_r+0x62>
        && (!(fp->_flags & __SSTR) || (fp->_flags & __SMBF)))
    __smakebuf_r (ptr, fp);

  if (fp->_flags & __SLBF)
 804ade2:	a8 01                	test   $0x1,%al
 804ade4:	75 5e                	jne    804ae44 <__swsetup_r+0x8c>
       */
      fp->_w = 0;
      fp->_lbfsize = -fp->_bf._size;
    }
  else
    fp->_w = fp->_flags & __SNBF ? 0 : fp->_bf._size;
 804ade6:	a8 02                	test   $0x2,%al
 804ade8:	0f 85 92 00 00 00    	jne    804ae80 <__swsetup_r+0xc8>
 804adee:	8b 43 14             	mov    0x14(%ebx),%eax
 804adf1:	89 43 08             	mov    %eax,0x8(%ebx)

  if (!fp->_bf._base && (fp->_flags & __SMBF))
 804adf4:	85 d2                	test   %edx,%edx
 804adf6:	74 5f                	je     804ae57 <__swsetup_r+0x9f>
    {
      /* __smakebuf_r set errno, but not flag */
      fp->_flags |= __SERR;
      return EOF;
    }
  return 0;
 804adf8:	31 c0                	xor    %eax,%eax
}
 804adfa:	83 c4 18             	add    $0x18,%esp
 804adfd:	5b                   	pop    %ebx
 804adfe:	c3                   	ret    
 804adff:	90                   	nop
   * If we are not writing, we had better be reading and writing.
   */

  if ((fp->_flags & __SWR) == 0)
    {
      if ((fp->_flags & __SRW) == 0)
 804ae00:	a8 10                	test   $0x10,%al
 804ae02:	0f 84 bc 00 00 00    	je     804aec4 <__swsetup_r+0x10c>
        {
	  ptr->_errno = EBADF;
	  fp->_flags |= __SERR;
	  return EOF;
        }
      if (fp->_flags & __SRD)
 804ae08:	a8 04                	test   $0x4,%al
 804ae0a:	75 7c                	jne    804ae88 <__swsetup_r+0xd0>
 804ae0c:	8b 53 10             	mov    0x10(%ebx),%edx
	    FREEUB (ptr, fp);
	  fp->_flags &= ~(__SRD | __SEOF);
	  fp->_r = 0;
	  fp->_p = fp->_bf._base;
	}
      fp->_flags |= __SWR;
 804ae0f:	83 c8 08             	or     $0x8,%eax
 804ae12:	66 89 43 0c          	mov    %ax,0xc(%ebx)
  /*
   * Make a buffer if necessary, then set _w.
   * A string I/O file should not explicitly allocate a buffer
   * unless asprintf is being used.
   */
  if (fp->_bf._base == NULL
 804ae16:	85 d2                	test   %edx,%edx
 804ae18:	75 c8                	jne    804ade2 <__swsetup_r+0x2a>
        && (!(fp->_flags & __SSTR) || (fp->_flags & __SMBF)))
 804ae1a:	89 c1                	mov    %eax,%ecx
 804ae1c:	81 e1 80 02 00 00    	and    $0x280,%ecx
 804ae22:	66 81 f9 00 02       	cmp    $0x200,%cx
 804ae27:	74 b9                	je     804ade2 <__swsetup_r+0x2a>
    __smakebuf_r (ptr, fp);
 804ae29:	89 5c 24 04          	mov    %ebx,0x4(%esp)
 804ae2d:	8b 54 24 20          	mov    0x20(%esp),%edx
 804ae31:	89 14 24             	mov    %edx,(%esp)
 804ae34:	e8 27 25 00 00       	call   804d360 <__smakebuf_r>
 804ae39:	8b 43 0c             	mov    0xc(%ebx),%eax
 804ae3c:	8b 53 10             	mov    0x10(%ebx),%edx
 804ae3f:	eb a1                	jmp    804ade2 <__swsetup_r+0x2a>
 804ae41:	8d 76 00             	lea    0x0(%esi),%esi
      /*
       * It is line buffered, so make _lbfsize be -_bufsize
       * for the putc() macro.  We will change _lbfsize back
       * to 0 whenever we turn off __SWR.
       */
      fp->_w = 0;
 804ae44:	c7 43 08 00 00 00 00 	movl   $0x0,0x8(%ebx)
      fp->_lbfsize = -fp->_bf._size;
 804ae4b:	8b 43 14             	mov    0x14(%ebx),%eax
 804ae4e:	f7 d8                	neg    %eax
 804ae50:	89 43 18             	mov    %eax,0x18(%ebx)
    }
  else
    fp->_w = fp->_flags & __SNBF ? 0 : fp->_bf._size;

  if (!fp->_bf._base && (fp->_flags & __SMBF))
 804ae53:	85 d2                	test   %edx,%edx
 804ae55:	75 a1                	jne    804adf8 <__swsetup_r+0x40>
 804ae57:	8b 53 0c             	mov    0xc(%ebx),%edx
    {
      /* __smakebuf_r set errno, but not flag */
      fp->_flags |= __SERR;
      return EOF;
    }
  return 0;
 804ae5a:	31 c0                	xor    %eax,%eax
      fp->_lbfsize = -fp->_bf._size;
    }
  else
    fp->_w = fp->_flags & __SNBF ? 0 : fp->_bf._size;

  if (!fp->_bf._base && (fp->_flags & __SMBF))
 804ae5c:	f6 c2 80             	test   $0x80,%dl
 804ae5f:	74 99                	je     804adfa <__swsetup_r+0x42>
    {
      /* __smakebuf_r set errno, but not flag */
      fp->_flags |= __SERR;
 804ae61:	83 ca 40             	or     $0x40,%edx
 804ae64:	66 89 53 0c          	mov    %dx,0xc(%ebx)
      return EOF;
 804ae68:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
 804ae6d:	eb 8b                	jmp    804adfa <__swsetup_r+0x42>
 804ae6f:	90                   	nop
       struct _reent *ptr _AND
       register FILE * fp)
{
  /* Make sure stdio is set up.  */

  CHECK_INIT (_REENT, fp);
 804ae70:	89 04 24             	mov    %eax,(%esp)
 804ae73:	e8 ec 19 00 00       	call   804c864 <__sinit>
 804ae78:	e9 57 ff ff ff       	jmp    804add4 <__swsetup_r+0x1c>
 804ae7d:	8d 76 00             	lea    0x0(%esi),%esi
       */
      fp->_w = 0;
      fp->_lbfsize = -fp->_bf._size;
    }
  else
    fp->_w = fp->_flags & __SNBF ? 0 : fp->_bf._size;
 804ae80:	31 c0                	xor    %eax,%eax
 804ae82:	e9 6a ff ff ff       	jmp    804adf1 <__swsetup_r+0x39>
 804ae87:	90                   	nop
	  return EOF;
        }
      if (fp->_flags & __SRD)
	{
	  /* clobber any ungetc data */
	  if (HASUB (fp))
 804ae88:	8b 53 30             	mov    0x30(%ebx),%edx
 804ae8b:	85 d2                	test   %edx,%edx
 804ae8d:	74 21                	je     804aeb0 <__swsetup_r+0xf8>
	    FREEUB (ptr, fp);
 804ae8f:	8d 4b 40             	lea    0x40(%ebx),%ecx
 804ae92:	39 ca                	cmp    %ecx,%edx
 804ae94:	74 13                	je     804aea9 <__swsetup_r+0xf1>
 804ae96:	89 54 24 04          	mov    %edx,0x4(%esp)
 804ae9a:	8b 44 24 20          	mov    0x20(%esp),%eax
 804ae9e:	89 04 24             	mov    %eax,(%esp)
 804aea1:	e8 5a 1c 00 00       	call   804cb00 <_free_r>
 804aea6:	8b 43 0c             	mov    0xc(%ebx),%eax
 804aea9:	c7 43 30 00 00 00 00 	movl   $0x0,0x30(%ebx)
	  fp->_flags &= ~(__SRD | __SEOF);
 804aeb0:	83 e0 db             	and    $0xffffffdb,%eax
	  fp->_r = 0;
 804aeb3:	c7 43 04 00 00 00 00 	movl   $0x0,0x4(%ebx)
	  fp->_p = fp->_bf._base;
 804aeba:	8b 53 10             	mov    0x10(%ebx),%edx
 804aebd:	89 13                	mov    %edx,(%ebx)
 804aebf:	e9 4b ff ff ff       	jmp    804ae0f <__swsetup_r+0x57>

  if ((fp->_flags & __SWR) == 0)
    {
      if ((fp->_flags & __SRW) == 0)
        {
	  ptr->_errno = EBADF;
 804aec4:	8b 54 24 20          	mov    0x20(%esp),%edx
 804aec8:	c7 02 09 00 00 00    	movl   $0x9,(%edx)
	  fp->_flags |= __SERR;
 804aece:	83 c8 40             	or     $0x40,%eax
 804aed1:	66 89 43 0c          	mov    %ax,0xc(%ebx)
	  return EOF;
 804aed5:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
 804aeda:	e9 1b ff ff ff       	jmp    804adfa <__swsetup_r+0x42>
 804aedf:	90                   	nop

0804aee0 <__call_exitprocs>:
 */

void 
_DEFUN (__call_exitprocs, (code, d),
	int code _AND _PTR d)
{
 804aee0:	55                   	push   %ebp
 804aee1:	57                   	push   %edi
 804aee2:	56                   	push   %esi
 804aee3:	53                   	push   %ebx
 804aee4:	83 ec 3c             	sub    $0x3c,%esp
 804aee7:	8b 6c 24 54          	mov    0x54(%esp),%ebp
 804aeeb:	a1 8c 16 05 08       	mov    0x805168c,%eax
 804aef0:	05 48 01 00 00       	add    $0x148,%eax
 804aef5:	89 44 24 2c          	mov    %eax,0x2c(%esp)
  __lock_acquire_recursive(__atexit_lock);
#endif

 restart:

  p = _GLOBAL_ATEXIT;
 804aef9:	8b 15 8c 16 05 08    	mov    0x805168c,%edx
 804aeff:	8b ba 48 01 00 00    	mov    0x148(%edx),%edi
  lastp = &_GLOBAL_ATEXIT;
 804af05:	8b 44 24 2c          	mov    0x2c(%esp),%eax
 804af09:	89 44 24 28          	mov    %eax,0x28(%esp)
  while (p)
 804af0d:	85 ff                	test   %edi,%edi
 804af0f:	0f 84 b0 00 00 00    	je     804afc5 <__call_exitprocs+0xe5>
#ifdef _REENT_SMALL
      args = p->_on_exit_args_ptr;
#else
      args = &p->_on_exit_args;
#endif
      for (n = p->_ind - 1; n >= 0; n--)
 804af15:	8b 5f 04             	mov    0x4(%edi),%ebx
 804af18:	4b                   	dec    %ebx
 804af19:	0f 88 9d 00 00 00    	js     804afbc <__call_exitprocs+0xdc>
 * Call registered exit handlers.  If D is null then all handlers are called,
 * otherwise only the handlers from that DSO are called.
 */

void 
_DEFUN (__call_exitprocs, (code, d),
 804af1f:	8d 97 88 00 00 00    	lea    0x88(%edi),%edx
 804af25:	89 54 24 24          	mov    %edx,0x24(%esp)
 804af29:	8d 34 9a             	lea    (%edx,%ebx,4),%esi
 804af2c:	eb 13                	jmp    804af41 <__call_exitprocs+0x61>
 804af2e:	66 90                	xchg   %ax,%ax
	  int ind;

	  i = 1 << n;

	  /* Skip functions not from this dso.  */
	  if (d && (!args || args->_dso_handle[n] != d))
 804af30:	39 ae 80 00 00 00    	cmp    %ebp,0x80(%esi)
 804af36:	74 0d                	je     804af45 <__call_exitprocs+0x65>
#ifdef _REENT_SMALL
      args = p->_on_exit_args_ptr;
#else
      args = &p->_on_exit_args;
#endif
      for (n = p->_ind - 1; n >= 0; n--)
 804af38:	4b                   	dec    %ebx
 804af39:	83 ee 04             	sub    $0x4,%esi
 804af3c:	83 fb ff             	cmp    $0xffffffff,%ebx
 804af3f:	74 7b                	je     804afbc <__call_exitprocs+0xdc>
	  int ind;

	  i = 1 << n;

	  /* Skip functions not from this dso.  */
	  if (d && (!args || args->_dso_handle[n] != d))
 804af41:	85 ed                	test   %ebp,%ebp
 804af43:	75 eb                	jne    804af30 <__call_exitprocs+0x50>
 * Call registered exit handlers.  If D is null then all handlers are called,
 * otherwise only the handlers from that DSO are called.
 */

void 
_DEFUN (__call_exitprocs, (code, d),
 804af45:	89 f2                	mov    %esi,%edx
 804af47:	2b 54 24 24          	sub    0x24(%esp),%edx
	  if (d && (!args || args->_dso_handle[n] != d))
	    continue;

	  /* Remove the function now to protect against the
	     function calling exit recursively.  */
	  fn = p->_fns[n];
 804af4b:	8b 44 3a 08          	mov    0x8(%edx,%edi,1),%eax
	  if (n == p->_ind - 1)
 804af4f:	8b 4f 04             	mov    0x4(%edi),%ecx
 804af52:	49                   	dec    %ecx
 804af53:	39 d9                	cmp    %ebx,%ecx
 804af55:	74 7d                	je     804afd4 <__call_exitprocs+0xf4>
	    p->_ind--;
	  else
	    p->_fns[n] = NULL;
 804af57:	c7 44 3a 08 00 00 00 	movl   $0x0,0x8(%edx,%edi,1)
 804af5e:	00 

	  /* Skip functions that have already been called.  */
	  if (!fn)
 804af5f:	85 c0                	test   %eax,%eax
 804af61:	74 d5                	je     804af38 <__call_exitprocs+0x58>
	    continue;

	  ind = p->_ind;
 804af63:	8b 57 04             	mov    0x4(%edi),%edx
 804af66:	89 54 24 18          	mov    %edx,0x18(%esp)
#endif
      for (n = p->_ind - 1; n >= 0; n--)
	{
	  int ind;

	  i = 1 << n;
 804af6a:	ba 01 00 00 00       	mov    $0x1,%edx
 804af6f:	88 d9                	mov    %bl,%cl
 804af71:	d3 e2                	shl    %cl,%edx
 804af73:	89 54 24 1c          	mov    %edx,0x1c(%esp)
	    continue;

	  ind = p->_ind;

	  /* Call the function.  */
	  if (!args || (args->_fntypes & i) == 0)
 804af77:	85 97 88 01 00 00    	test   %edx,0x188(%edi)
 804af7d:	74 51                	je     804afd0 <__call_exitprocs+0xf0>
	    fn ();
	  else if ((args->_is_cxa & i) == 0)
 804af7f:	8b 4c 24 1c          	mov    0x1c(%esp),%ecx
 804af83:	85 8f 8c 01 00 00    	test   %ecx,0x18c(%edi)
	    (*((void (*)(int, _PTR)) fn))(code, args->_fnargs[n]);
 804af89:	8b 0e                	mov    (%esi),%ecx
	  ind = p->_ind;

	  /* Call the function.  */
	  if (!args || (args->_fntypes & i) == 0)
	    fn ();
	  else if ((args->_is_cxa & i) == 0)
 804af8b:	75 4f                	jne    804afdc <__call_exitprocs+0xfc>
	    (*((void (*)(int, _PTR)) fn))(code, args->_fnargs[n]);
 804af8d:	89 4c 24 04          	mov    %ecx,0x4(%esp)
 804af91:	8b 54 24 50          	mov    0x50(%esp),%edx
 804af95:	89 14 24             	mov    %edx,(%esp)
 804af98:	ff d0                	call   *%eax
	    (*((void (*)(_PTR)) fn))(args->_fnargs[n]);

	  /* The function we called call atexit and registered another
	     function (or functions).  Call these new functions before
	     continuing with the already registered functions.  */
	  if (ind != p->_ind || *lastp != p)
 804af9a:	8b 44 24 18          	mov    0x18(%esp),%eax
 804af9e:	39 47 04             	cmp    %eax,0x4(%edi)
 804afa1:	0f 85 52 ff ff ff    	jne    804aef9 <__call_exitprocs+0x19>
 804afa7:	8b 54 24 28          	mov    0x28(%esp),%edx
 804afab:	39 3a                	cmp    %edi,(%edx)
 804afad:	0f 85 46 ff ff ff    	jne    804aef9 <__call_exitprocs+0x19>
#ifdef _REENT_SMALL
      args = p->_on_exit_args_ptr;
#else
      args = &p->_on_exit_args;
#endif
      for (n = p->_ind - 1; n >= 0; n--)
 804afb3:	4b                   	dec    %ebx
 804afb4:	83 ee 04             	sub    $0x4,%esi
 804afb7:	83 fb ff             	cmp    $0xffffffff,%ebx
 804afba:	75 85                	jne    804af41 <__call_exitprocs+0x61>
#ifndef _ATEXIT_DYNAMIC_ALLOC
      break;
#else
      /* Don't dynamically free the atexit array if free is not
	 available.  */
      if (!free)
 804afbc:	b9 a0 d4 04 08       	mov    $0x804d4a0,%ecx
 804afc1:	85 c9                	test   %ecx,%ecx
 804afc3:	75 1e                	jne    804afe3 <__call_exitprocs+0x103>
    }
#ifndef __SINGLE_THREAD__
  __lock_release_recursive(__atexit_lock);
#endif

}
 804afc5:	83 c4 3c             	add    $0x3c,%esp
 804afc8:	5b                   	pop    %ebx
 804afc9:	5e                   	pop    %esi
 804afca:	5f                   	pop    %edi
 804afcb:	5d                   	pop    %ebp
 804afcc:	c3                   	ret    
 804afcd:	8d 76 00             	lea    0x0(%esi),%esi

	  ind = p->_ind;

	  /* Call the function.  */
	  if (!args || (args->_fntypes & i) == 0)
	    fn ();
 804afd0:	ff d0                	call   *%eax
 804afd2:	eb c6                	jmp    804af9a <__call_exitprocs+0xba>

	  /* Remove the function now to protect against the
	     function calling exit recursively.  */
	  fn = p->_fns[n];
	  if (n == p->_ind - 1)
	    p->_ind--;
 804afd4:	89 5f 04             	mov    %ebx,0x4(%edi)
 804afd7:	eb 86                	jmp    804af5f <__call_exitprocs+0x7f>
 804afd9:	8d 76 00             	lea    0x0(%esi),%esi
	  if (!args || (args->_fntypes & i) == 0)
	    fn ();
	  else if ((args->_is_cxa & i) == 0)
	    (*((void (*)(int, _PTR)) fn))(code, args->_fnargs[n]);
	  else
	    (*((void (*)(_PTR)) fn))(args->_fnargs[n]);
 804afdc:	89 0c 24             	mov    %ecx,(%esp)
 804afdf:	ff d0                	call   *%eax
 804afe1:	eb b7                	jmp    804af9a <__call_exitprocs+0xba>
      if (!free)
	break;

      /* Move to the next block.  Free empty blocks except the last one,
	 which is part of _GLOBAL_REENT.  */
      if (p->_ind == 0 && p->_next)
 804afe3:	8b 47 04             	mov    0x4(%edi),%eax
 804afe6:	85 c0                	test   %eax,%eax
 804afe8:	8b 07                	mov    (%edi),%eax
 804afea:	75 1f                	jne    804b00b <__call_exitprocs+0x12b>
 804afec:	85 c0                	test   %eax,%eax
 804afee:	74 19                	je     804b009 <__call_exitprocs+0x129>
	{
	  /* Remove empty block from the list.  */
	  *lastp = p->_next;
 804aff0:	8b 54 24 28          	mov    0x28(%esp),%edx
 804aff4:	89 02                	mov    %eax,(%edx)
#ifdef _REENT_SMALL
	  if (args)
	    free (args);
#endif
	  free (p);
 804aff6:	89 3c 24             	mov    %edi,(%esp)
 804aff9:	e8 a2 24 00 00       	call   804d4a0 <free>
	  p = *lastp;
 804affe:	8b 44 24 28          	mov    0x28(%esp),%eax
 804b002:	8b 38                	mov    (%eax),%edi
 804b004:	e9 04 ff ff ff       	jmp    804af0d <__call_exitprocs+0x2d>
      if (!free)
	break;

      /* Move to the next block.  Free empty blocks except the last one,
	 which is part of _GLOBAL_REENT.  */
      if (p->_ind == 0 && p->_next)
 804b009:	31 c0                	xor    %eax,%eax
	  free (p);
	  p = *lastp;
	}
      else
	{
	  lastp = &p->_next;
 804b00b:	89 7c 24 28          	mov    %edi,0x28(%esp)
	  p = p->_next;
 804b00f:	89 c7                	mov    %eax,%edi
 804b011:	e9 f7 fe ff ff       	jmp    804af0d <__call_exitprocs+0x2d>
 804b016:	90                   	nop
 804b017:	90                   	nop

0804b018 <quorem>:

static int
_DEFUN (quorem,
	(b, S),
	_Bigint * b _AND _Bigint * S)
{
 804b018:	55                   	push   %ebp
 804b019:	57                   	push   %edi
 804b01a:	56                   	push   %esi
 804b01b:	53                   	push   %ebx
 804b01c:	83 ec 4c             	sub    $0x4c,%esp
 804b01f:	89 44 24 24          	mov    %eax,0x24(%esp)
 804b023:	89 54 24 28          	mov    %edx,0x28(%esp)
#ifdef Pack_32
  __Long z;
  __ULong si, zs;
#endif

  n = S->_wds;
 804b027:	8b 42 10             	mov    0x10(%edx),%eax
#ifdef DEBUG
  /*debug*/ if (b->_wds > n)
    /*debug*/ Bug ("oversize b in quorem");
#endif
  if (b->_wds < n)
 804b02a:	8b 54 24 24          	mov    0x24(%esp),%edx
 804b02e:	3b 42 10             	cmp    0x10(%edx),%eax
 804b031:	0f 8f b5 01 00 00    	jg     804b1ec <quorem+0x1d4>
    return 0;
  sx = S->_x;
 804b037:	8b 6c 24 28          	mov    0x28(%esp),%ebp
 804b03b:	83 c5 14             	add    $0x14,%ebp
  sxe = sx + --n;
 804b03e:	8d 78 ff             	lea    -0x1(%eax),%edi
 804b041:	83 c0 03             	add    $0x3,%eax
 804b044:	8b 54 24 28          	mov    0x28(%esp),%edx
 804b048:	8d 54 82 04          	lea    0x4(%edx,%eax,4),%edx
 804b04c:	89 54 24 2c          	mov    %edx,0x2c(%esp)
  bx = b->_x;
 804b050:	8b 54 24 24          	mov    0x24(%esp),%edx
 804b054:	83 c2 14             	add    $0x14,%edx
 804b057:	89 54 24 30          	mov    %edx,0x30(%esp)
  bxe = bx + n;
  q = *bxe / (*sxe + 1);	/* ensure q <= true quotient */
 804b05b:	8b 54 24 28          	mov    0x28(%esp),%edx
 804b05f:	8b 54 82 04          	mov    0x4(%edx,%eax,4),%edx
 804b063:	89 d1                	mov    %edx,%ecx
 804b065:	41                   	inc    %ecx
 804b066:	8b 54 24 24          	mov    0x24(%esp),%edx
 804b06a:	8b 44 82 04          	mov    0x4(%edx,%eax,4),%eax
 804b06e:	31 d2                	xor    %edx,%edx
 804b070:	f7 f1                	div    %ecx
 804b072:	89 44 24 20          	mov    %eax,0x20(%esp)
 804b076:	89 44 24 34          	mov    %eax,0x34(%esp)
#ifdef DEBUG
  /*debug*/ if (q > 9)
    /*debug*/ Bug ("oversized quotient in quorem");
#endif
  if (q)
 804b07a:	85 c0                	test   %eax,%eax
 804b07c:	0f 84 b7 00 00 00    	je     804b139 <quorem+0x121>
 804b082:	89 eb                	mov    %ebp,%ebx
 804b084:	8b 4c 24 30          	mov    0x30(%esp),%ecx
 804b088:	31 d2                	xor    %edx,%edx
 804b08a:	31 c0                	xor    %eax,%eax
 804b08c:	89 6c 24 38          	mov    %ebp,0x38(%esp)
 804b090:	89 d5                	mov    %edx,%ebp
 804b092:	89 7c 24 3c          	mov    %edi,0x3c(%esp)
 804b096:	89 c7                	mov    %eax,%edi
      borrow = 0;
      carry = 0;
      do
	{
#ifdef Pack_32
	  si = *sx++;
 804b098:	8b 13                	mov    (%ebx),%edx
 804b09a:	83 c3 04             	add    $0x4,%ebx
	  ys = (si & 0xffff) * q + carry;
 804b09d:	0f b7 f2             	movzwl %dx,%esi
 804b0a0:	0f af 74 24 20       	imul   0x20(%esp),%esi
 804b0a5:	01 ee                	add    %ebp,%esi
	  zs = (si >> 16) * q + (ys >> 16);
 804b0a7:	89 f0                	mov    %esi,%eax
 804b0a9:	c1 e8 10             	shr    $0x10,%eax
 804b0ac:	c1 ea 10             	shr    $0x10,%edx
 804b0af:	0f af 54 24 20       	imul   0x20(%esp),%edx
 804b0b4:	01 c2                	add    %eax,%edx
	  carry = zs >> 16;
 804b0b6:	89 d5                	mov    %edx,%ebp
 804b0b8:	c1 ed 10             	shr    $0x10,%ebp
	  y = (*bx & 0xffff) - (ys & 0xffff) + borrow;
 804b0bb:	0f b7 01             	movzwl (%ecx),%eax
 804b0be:	01 f8                	add    %edi,%eax
 804b0c0:	81 e6 ff ff 00 00    	and    $0xffff,%esi
 804b0c6:	29 f0                	sub    %esi,%eax
	  borrow = y >> 16;
	  Sign_Extend (borrow, y);
	  z = (*bx >> 16) - (zs & 0xffff) + borrow;
 804b0c8:	8b 31                	mov    (%ecx),%esi
 804b0ca:	c1 ee 10             	shr    $0x10,%esi
 804b0cd:	81 e2 ff ff 00 00    	and    $0xffff,%edx
 804b0d3:	29 d6                	sub    %edx,%esi
	  si = *sx++;
	  ys = (si & 0xffff) * q + carry;
	  zs = (si >> 16) * q + (ys >> 16);
	  carry = zs >> 16;
	  y = (*bx & 0xffff) - (ys & 0xffff) + borrow;
	  borrow = y >> 16;
 804b0d5:	89 c2                	mov    %eax,%edx
 804b0d7:	c1 fa 10             	sar    $0x10,%edx
	  Sign_Extend (borrow, y);
	  z = (*bx >> 16) - (zs & 0xffff) + borrow;
 804b0da:	01 f2                	add    %esi,%edx
	  borrow = z >> 16;
 804b0dc:	89 d7                	mov    %edx,%edi
 804b0de:	c1 ff 10             	sar    $0x10,%edi
	  Sign_Extend (borrow, z);
	  Storeinc (bx, z, y);
 804b0e1:	c1 e2 10             	shl    $0x10,%edx
 804b0e4:	25 ff ff 00 00       	and    $0xffff,%eax
 804b0e9:	09 c2                	or     %eax,%edx
 804b0eb:	89 11                	mov    %edx,(%ecx)
 804b0ed:	83 c1 04             	add    $0x4,%ecx
	  borrow = y >> 16;
	  Sign_Extend (borrow, y);
	  *bx++ = y & 0xffff;
#endif
	}
      while (sx <= sxe);
 804b0f0:	39 5c 24 2c          	cmp    %ebx,0x2c(%esp)
 804b0f4:	73 a2                	jae    804b098 <quorem+0x80>
 804b0f6:	8b 6c 24 38          	mov    0x38(%esp),%ebp
 804b0fa:	8b 7c 24 3c          	mov    0x3c(%esp),%edi
      if (!*bxe)
 804b0fe:	8d 47 04             	lea    0x4(%edi),%eax
 804b101:	8b 54 24 24          	mov    0x24(%esp),%edx
 804b105:	8b 74 82 04          	mov    0x4(%edx,%eax,4),%esi
 804b109:	85 f6                	test   %esi,%esi
 804b10b:	75 2c                	jne    804b139 <quorem+0x121>
  if (b->_wds < n)
    return 0;
  sx = S->_x;
  sxe = sx + --n;
  bx = b->_x;
  bxe = bx + n;
 804b10d:	8d 04 82             	lea    (%edx,%eax,4),%eax
	}
      while (sx <= sxe);
      if (!*bxe)
	{
	  bx = b->_x;
	  while (--bxe > bx && !*bxe)
 804b110:	39 44 24 30          	cmp    %eax,0x30(%esp)
 804b114:	73 1c                	jae    804b132 <quorem+0x11a>
 804b116:	8b 18                	mov    (%eax),%ebx
 804b118:	85 db                	test   %ebx,%ebx
 804b11a:	75 16                	jne    804b132 <quorem+0x11a>
 804b11c:	8b 54 24 30          	mov    0x30(%esp),%edx
 804b120:	eb 08                	jmp    804b12a <quorem+0x112>
 804b122:	66 90                	xchg   %ax,%ax
 804b124:	8b 08                	mov    (%eax),%ecx
 804b126:	85 c9                	test   %ecx,%ecx
 804b128:	75 08                	jne    804b132 <quorem+0x11a>
	    --n;
 804b12a:	4f                   	dec    %edi
	}
      while (sx <= sxe);
      if (!*bxe)
	{
	  bx = b->_x;
	  while (--bxe > bx && !*bxe)
 804b12b:	83 e8 04             	sub    $0x4,%eax
 804b12e:	39 c2                	cmp    %eax,%edx
 804b130:	72 f2                	jb     804b124 <quorem+0x10c>
	    --n;
	  b->_wds = n;
 804b132:	8b 44 24 24          	mov    0x24(%esp),%eax
 804b136:	89 78 10             	mov    %edi,0x10(%eax)
	}
    }
  if (cmp (b, S) >= 0)
 804b139:	8b 54 24 28          	mov    0x28(%esp),%edx
 804b13d:	89 54 24 04          	mov    %edx,0x4(%esp)
 804b141:	8b 44 24 24          	mov    0x24(%esp),%eax
 804b145:	89 04 24             	mov    %eax,(%esp)
 804b148:	e8 73 33 00 00       	call   804e4c0 <__mcmp>
 804b14d:	85 c0                	test   %eax,%eax
 804b14f:	0f 88 88 00 00 00    	js     804b1dd <quorem+0x1c5>
    {
      q++;
 804b155:	8b 54 24 20          	mov    0x20(%esp),%edx
 804b159:	42                   	inc    %edx
 804b15a:	89 54 24 34          	mov    %edx,0x34(%esp)
#endif
  if (b->_wds < n)
    return 0;
  sx = S->_x;
  sxe = sx + --n;
  bx = b->_x;
 804b15e:	8b 54 24 30          	mov    0x30(%esp),%edx
	}
    }
  if (cmp (b, S) >= 0)
    {
      q++;
      borrow = 0;
 804b162:	31 db                	xor    %ebx,%ebx
 804b164:	8b 74 24 2c          	mov    0x2c(%esp),%esi
      bx = b->_x;
      sx = S->_x;
      do
	{
#ifdef Pack_32
	  si = *sx++;
 804b168:	8b 4d 00             	mov    0x0(%ebp),%ecx
 804b16b:	83 c5 04             	add    $0x4,%ebp
	  ys = (si & 0xffff) + carry;
	  zs = (si >> 16) + (ys >> 16);
	  carry = zs >> 16;
	  y = (*bx & 0xffff) - (ys & 0xffff) + borrow;
 804b16e:	0f b7 02             	movzwl (%edx),%eax
 804b171:	01 d8                	add    %ebx,%eax
 804b173:	0f b7 d9             	movzwl %cx,%ebx
 804b176:	29 d8                	sub    %ebx,%eax
	  borrow = y >> 16;
	  Sign_Extend (borrow, y);
	  z = (*bx >> 16) - (zs & 0xffff) + borrow;
 804b178:	8b 1a                	mov    (%edx),%ebx
 804b17a:	c1 eb 10             	shr    $0x10,%ebx
 804b17d:	c1 e9 10             	shr    $0x10,%ecx
 804b180:	29 cb                	sub    %ecx,%ebx
	  si = *sx++;
	  ys = (si & 0xffff) + carry;
	  zs = (si >> 16) + (ys >> 16);
	  carry = zs >> 16;
	  y = (*bx & 0xffff) - (ys & 0xffff) + borrow;
	  borrow = y >> 16;
 804b182:	89 c1                	mov    %eax,%ecx
 804b184:	c1 f9 10             	sar    $0x10,%ecx
	  Sign_Extend (borrow, y);
	  z = (*bx >> 16) - (zs & 0xffff) + borrow;
 804b187:	01 d9                	add    %ebx,%ecx
	  borrow = z >> 16;
 804b189:	89 cb                	mov    %ecx,%ebx
 804b18b:	c1 fb 10             	sar    $0x10,%ebx
	  Sign_Extend (borrow, z);
	  Storeinc (bx, z, y);
 804b18e:	c1 e1 10             	shl    $0x10,%ecx
 804b191:	25 ff ff 00 00       	and    $0xffff,%eax
 804b196:	09 c1                	or     %eax,%ecx
 804b198:	89 0a                	mov    %ecx,(%edx)
 804b19a:	83 c2 04             	add    $0x4,%edx
	  borrow = y >> 16;
	  Sign_Extend (borrow, y);
	  *bx++ = y & 0xffff;
#endif
	}
      while (sx <= sxe);
 804b19d:	39 ee                	cmp    %ebp,%esi
 804b19f:	73 c7                	jae    804b168 <quorem+0x150>
      bx = b->_x;
      bxe = bx + n;
      if (!*bxe)
 804b1a1:	8d 47 04             	lea    0x4(%edi),%eax
 804b1a4:	8b 54 24 24          	mov    0x24(%esp),%edx
 804b1a8:	8b 74 82 04          	mov    0x4(%edx,%eax,4),%esi
 804b1ac:	85 f6                	test   %esi,%esi
 804b1ae:	75 2d                	jne    804b1dd <quorem+0x1c5>
	  *bx++ = y & 0xffff;
#endif
	}
      while (sx <= sxe);
      bx = b->_x;
      bxe = bx + n;
 804b1b0:	8d 04 82             	lea    (%edx,%eax,4),%eax
      if (!*bxe)
	{
	  while (--bxe > bx && !*bxe)
 804b1b3:	39 44 24 30          	cmp    %eax,0x30(%esp)
 804b1b7:	73 1d                	jae    804b1d6 <quorem+0x1be>
 804b1b9:	8b 18                	mov    (%eax),%ebx
 804b1bb:	85 db                	test   %ebx,%ebx
 804b1bd:	75 17                	jne    804b1d6 <quorem+0x1be>
 804b1bf:	8b 54 24 30          	mov    0x30(%esp),%edx
 804b1c3:	eb 09                	jmp    804b1ce <quorem+0x1b6>
 804b1c5:	8d 76 00             	lea    0x0(%esi),%esi
 804b1c8:	8b 08                	mov    (%eax),%ecx
 804b1ca:	85 c9                	test   %ecx,%ecx
 804b1cc:	75 08                	jne    804b1d6 <quorem+0x1be>
	    --n;
 804b1ce:	4f                   	dec    %edi
      while (sx <= sxe);
      bx = b->_x;
      bxe = bx + n;
      if (!*bxe)
	{
	  while (--bxe > bx && !*bxe)
 804b1cf:	83 e8 04             	sub    $0x4,%eax
 804b1d2:	39 c2                	cmp    %eax,%edx
 804b1d4:	72 f2                	jb     804b1c8 <quorem+0x1b0>
	    --n;
	  b->_wds = n;
 804b1d6:	8b 44 24 24          	mov    0x24(%esp),%eax
 804b1da:	89 78 10             	mov    %edi,0x10(%eax)
	}
    }
  return q;
 804b1dd:	8b 44 24 34          	mov    0x34(%esp),%eax
}
 804b1e1:	83 c4 4c             	add    $0x4c,%esp
 804b1e4:	5b                   	pop    %ebx
 804b1e5:	5e                   	pop    %esi
 804b1e6:	5f                   	pop    %edi
 804b1e7:	5d                   	pop    %ebp
 804b1e8:	c3                   	ret    
 804b1e9:	8d 76 00             	lea    0x0(%esi),%esi
#ifdef DEBUG
  /*debug*/ if (b->_wds > n)
    /*debug*/ Bug ("oversize b in quorem");
#endif
  if (b->_wds < n)
    return 0;
 804b1ec:	31 c0                	xor    %eax,%eax
	    --n;
	  b->_wds = n;
	}
    }
  return q;
}
 804b1ee:	83 c4 4c             	add    $0x4c,%esp
 804b1f1:	5b                   	pop    %ebx
 804b1f2:	5e                   	pop    %esi
 804b1f3:	5f                   	pop    %edi
 804b1f4:	5d                   	pop    %ebp
 804b1f5:	c3                   	ret    
 804b1f6:	66 90                	xchg   %ax,%ax

0804b1f8 <_dtoa_r>:
	int mode _AND
	int ndigits _AND
	int *decpt _AND
	int *sign _AND
	char **rve)
{
 804b1f8:	55                   	push   %ebp
 804b1f9:	57                   	push   %edi
 804b1fa:	56                   	push   %esi
 804b1fb:	53                   	push   %ebx
 804b1fc:	81 ec 9c 00 00 00    	sub    $0x9c,%esp
 804b202:	8b ac 24 b0 00 00 00 	mov    0xb0(%esp),%ebp
 804b209:	8b 9c 24 c8 00 00 00 	mov    0xc8(%esp),%ebx
#endif
  _Bigint *b, *b1, *delta, *mlo = NULL, *mhi, *S;
  double ds;
  char *s, *s0;

  d.d = _d;
 804b210:	dd 84 24 b4 00 00 00 	fldl   0xb4(%esp)
 804b217:	dd 5c 24 30          	fstpl  0x30(%esp)

  _REENT_CHECK_MP(ptr);
  if (_REENT_MP_RESULT(ptr))
 804b21b:	8b 45 40             	mov    0x40(%ebp),%eax
 804b21e:	85 c0                	test   %eax,%eax
 804b220:	74 23                	je     804b245 <_dtoa_r+0x4d>
    {
      _REENT_MP_RESULT(ptr)->_k = _REENT_MP_RESULT_K(ptr);
 804b222:	8b 4d 44             	mov    0x44(%ebp),%ecx
 804b225:	89 48 04             	mov    %ecx,0x4(%eax)
      _REENT_MP_RESULT(ptr)->_maxwds = 1 << _REENT_MP_RESULT_K(ptr);
 804b228:	ba 01 00 00 00       	mov    $0x1,%edx
 804b22d:	d3 e2                	shl    %cl,%edx
 804b22f:	89 50 08             	mov    %edx,0x8(%eax)
      Bfree (ptr, _REENT_MP_RESULT(ptr));
 804b232:	89 44 24 04          	mov    %eax,0x4(%esp)
 804b236:	89 2c 24             	mov    %ebp,(%esp)
 804b239:	e8 fe 2b 00 00       	call   804de3c <_Bfree>
      _REENT_MP_RESULT(ptr) = 0;
 804b23e:	c7 45 40 00 00 00 00 	movl   $0x0,0x40(%ebp)
    }

  if (word0 (d) & Sign_bit)
 804b245:	8b 7c 24 34          	mov    0x34(%esp),%edi
 804b249:	85 ff                	test   %edi,%edi
 804b24b:	0f 88 bb 00 00 00    	js     804b30c <_dtoa_r+0x114>
      /* set sign for everything, including 0's and NaNs */
      *sign = 1;
      word0 (d) &= ~Sign_bit;	/* clear sign bit */
    }
  else
    *sign = 0;
 804b251:	c7 03 00 00 00 00    	movl   $0x0,(%ebx)

#if defined(IEEE_Arith) + defined(VAX)
#ifdef IEEE_Arith
  if ((word0 (d) & Exp_mask) == Exp_mask)
 804b257:	89 f8                	mov    %edi,%eax
 804b259:	25 00 00 f0 7f       	and    $0x7ff00000,%eax
 804b25e:	3d 00 00 f0 7f       	cmp    $0x7ff00000,%eax
 804b263:	74 5b                	je     804b2c0 <_dtoa_r+0xc8>
    }
#endif
#ifdef IBM
  d.d += 0;			/* normalize */
#endif
  if (!d.d)
 804b265:	dd 44 24 30          	fldl   0x30(%esp)
 804b269:	dd 5c 24 28          	fstpl  0x28(%esp)
 804b26d:	d9 ee                	fldz   
 804b26f:	dd 44 24 28          	fldl   0x28(%esp)
 804b273:	da e9                	fucompp 
 804b275:	df e0                	fnstsw %ax
 804b277:	80 e4 45             	and    $0x45,%ah
 804b27a:	80 f4 40             	xor    $0x40,%ah
 804b27d:	0f 85 a1 00 00 00    	jne    804b324 <_dtoa_r+0x12c>
    {
      *decpt = 1;
 804b283:	8b 84 24 c4 00 00 00 	mov    0xc4(%esp),%eax
 804b28a:	c7 00 01 00 00 00    	movl   $0x1,(%eax)
      s = "0";
      if (rve)
 804b290:	8b ac 24 cc 00 00 00 	mov    0xcc(%esp),%ebp
 804b297:	85 ed                	test   %ebp,%ebp
 804b299:	0f 84 bd 04 00 00    	je     804b75c <_dtoa_r+0x564>
	*rve = s + 1;
 804b29f:	8b 94 24 cc 00 00 00 	mov    0xcc(%esp),%edx
 804b2a6:	c7 02 ce 16 05 08    	movl   $0x80516ce,(%edx)
      return s;
 804b2ac:	be cd 16 05 08       	mov    $0x80516cd,%esi
  *s = 0;
  *decpt = k + 1;
  if (rve)
    *rve = s;
  return s0;
}
 804b2b1:	89 f0                	mov    %esi,%eax
 804b2b3:	81 c4 9c 00 00 00    	add    $0x9c,%esp
 804b2b9:	5b                   	pop    %ebx
 804b2ba:	5e                   	pop    %esi
 804b2bb:	5f                   	pop    %edi
 804b2bc:	5d                   	pop    %ebp
 804b2bd:	c3                   	ret    
 804b2be:	66 90                	xchg   %ax,%ax
#else
  if (word0 (d) == 0x8000)
#endif
    {
      /* Infinity or NaN */
      *decpt = 9999;
 804b2c0:	8b 84 24 c4 00 00 00 	mov    0xc4(%esp),%eax
 804b2c7:	c7 00 0f 27 00 00    	movl   $0x270f,(%eax)
      s =
#ifdef IEEE_Arith
	!word1 (d) && !(word0 (d) & 0xfffff) ? "Infinity" :
 804b2cd:	8b 44 24 30          	mov    0x30(%esp),%eax
 804b2d1:	85 c0                	test   %eax,%eax
 804b2d3:	0f 84 37 04 00 00    	je     804b710 <_dtoa_r+0x518>
 804b2d9:	be 5d 18 05 08       	mov    $0x805185d,%esi
#endif
	"NaN";
      if (rve)
 804b2de:	8b 84 24 cc 00 00 00 	mov    0xcc(%esp),%eax
 804b2e5:	85 c0                	test   %eax,%eax
 804b2e7:	74 c8                	je     804b2b1 <_dtoa_r+0xb9>
	*rve =
#ifdef IEEE_Arith
	  s[3] ? s + 8 :
 804b2e9:	80 7e 03 00          	cmpb   $0x0,0x3(%esi)
 804b2ed:	0f 85 35 04 00 00    	jne    804b728 <_dtoa_r+0x530>
 804b2f3:	8d 46 03             	lea    0x3(%esi),%eax
#ifdef IEEE_Arith
	!word1 (d) && !(word0 (d) & 0xfffff) ? "Infinity" :
#endif
	"NaN";
      if (rve)
	*rve =
 804b2f6:	8b 94 24 cc 00 00 00 	mov    0xcc(%esp),%edx
 804b2fd:	89 02                	mov    %eax,(%edx)
  *s = 0;
  *decpt = k + 1;
  if (rve)
    *rve = s;
  return s0;
}
 804b2ff:	89 f0                	mov    %esi,%eax
 804b301:	81 c4 9c 00 00 00    	add    $0x9c,%esp
 804b307:	5b                   	pop    %ebx
 804b308:	5e                   	pop    %esi
 804b309:	5f                   	pop    %edi
 804b30a:	5d                   	pop    %ebp
 804b30b:	c3                   	ret    
    }

  if (word0 (d) & Sign_bit)
    {
      /* set sign for everything, including 0's and NaNs */
      *sign = 1;
 804b30c:	c7 03 01 00 00 00    	movl   $0x1,(%ebx)
      word0 (d) &= ~Sign_bit;	/* clear sign bit */
 804b312:	81 e7 ff ff ff 7f    	and    $0x7fffffff,%edi
 804b318:	89 7c 24 34          	mov    %edi,0x34(%esp)
 804b31c:	e9 36 ff ff ff       	jmp    804b257 <_dtoa_r+0x5f>
 804b321:	8d 76 00             	lea    0x0(%esi),%esi
      if (rve)
	*rve = s + 1;
      return s;
    }

  b = d2b (ptr, d.d, &be, &bbits);
 804b324:	8d 84 24 88 00 00 00 	lea    0x88(%esp),%eax
 804b32b:	89 44 24 10          	mov    %eax,0x10(%esp)
 804b32f:	8d 84 24 8c 00 00 00 	lea    0x8c(%esp),%eax
 804b336:	89 44 24 0c          	mov    %eax,0xc(%esp)
 804b33a:	dd 44 24 28          	fldl   0x28(%esp)
 804b33e:	dd 5c 24 04          	fstpl  0x4(%esp)
 804b342:	89 2c 24             	mov    %ebp,(%esp)
 804b345:	e8 b6 34 00 00       	call   804e800 <__d2b>
 804b34a:	89 44 24 54          	mov    %eax,0x54(%esp)
#ifdef Sudden_Underflow
  i = (int) (word0 (d) >> Exp_shift1 & (Exp_mask >> Exp_shift1));
#else
  if ((i = (int) (word0 (d) >> Exp_shift1 & (Exp_mask >> Exp_shift1))) != 0)
 804b34e:	89 fe                	mov    %edi,%esi
 804b350:	c1 ee 14             	shr    $0x14,%esi
 804b353:	81 e6 ff 07 00 00    	and    $0x7ff,%esi
 804b359:	0f 85 d1 03 00 00    	jne    804b730 <_dtoa_r+0x538>
    }
  else
    {
      /* d is denormalized */

      i = bbits + be + (Bias + (P - 1) - 1);
 804b35f:	8b 9c 24 88 00 00 00 	mov    0x88(%esp),%ebx
 804b366:	8b b4 24 8c 00 00 00 	mov    0x8c(%esp),%esi
 804b36d:	01 de                	add    %ebx,%esi
#if defined (_DOUBLE_IS_32BITS)
      x = word0 (d) << (32 - i);
#else
      x = (i > 32) ? (word0 (d) << (64 - i)) | (word1 (d) >> (i - 32))
 804b36f:	81 fe ef fb ff ff    	cmp    $0xfffffbef,%esi
 804b375:	0f 8c 59 09 00 00    	jl     804bcd4 <_dtoa_r+0xadc>
 804b37b:	b9 0e fc ff ff       	mov    $0xfffffc0e,%ecx
 804b380:	29 f1                	sub    %esi,%ecx
 804b382:	89 f8                	mov    %edi,%eax
 804b384:	d3 e0                	shl    %cl,%eax
 804b386:	8d 8e 12 04 00 00    	lea    0x412(%esi),%ecx
 804b38c:	8b 54 24 30          	mov    0x30(%esp),%edx
 804b390:	d3 ea                	shr    %cl,%edx
 804b392:	09 d0                	or     %edx,%eax
       : (word1 (d) << (32 - i));
#endif
      d2.d = x;
 804b394:	31 d2                	xor    %edx,%edx
 804b396:	89 44 24 70          	mov    %eax,0x70(%esp)
 804b39a:	89 54 24 74          	mov    %edx,0x74(%esp)
 804b39e:	df 6c 24 70          	fildll 0x70(%esp)
 804b3a2:	dd 5c 24 28          	fstpl  0x28(%esp)
      word0 (d2) -= 31 * Exp_msk1;	/* adjust exponent */
 804b3a6:	81 6c 24 2c 00 00 f0 	subl   $0x1f00000,0x2c(%esp)
 804b3ad:	01 
      i -= (Bias + (P - 1) - 1) + 1;
 804b3ae:	4e                   	dec    %esi
      denorm = 1;
 804b3af:	c7 44 24 4c 01 00 00 	movl   $0x1,0x4c(%esp)
 804b3b6:	00 
    }
#endif
#if defined (_DOUBLE_IS_32BITS)
  ds = (d2.d - 1.5) * 0.289529651 + 0.176091269 + i * 0.30103001;
#else
  ds = (d2.d - 1.5) * 0.289529654602168 + 0.1760912590558 + i * 0.301029995663981;
 804b3b7:	dd 44 24 28          	fldl   0x28(%esp)
 804b3bb:	d8 25 64 18 05 08    	fsubs  0x8051864
 804b3c1:	dc 0d 80 18 05 08    	fmull  0x8051880
 804b3c7:	dc 05 88 18 05 08    	faddl  0x8051888
 804b3cd:	dd 05 90 18 05 08    	fldl   0x8051890
 804b3d3:	56                   	push   %esi
 804b3d4:	da 0c 24             	fimull (%esp)
 804b3d7:	5f                   	pop    %edi
 804b3d8:	de c1                	faddp  %st,%st(1)
#endif
  k = (int) ds;
 804b3da:	d9 7c 24 7e          	fnstcw 0x7e(%esp)
 804b3de:	66 8b 44 24 7e       	mov    0x7e(%esp),%ax
 804b3e3:	b4 0c                	mov    $0xc,%ah
 804b3e5:	66 89 44 24 7c       	mov    %ax,0x7c(%esp)
 804b3ea:	d9 6c 24 7c          	fldcw  0x7c(%esp)
 804b3ee:	db 54 24 44          	fistl  0x44(%esp)
 804b3f2:	d9 6c 24 7e          	fldcw  0x7e(%esp)
  if (ds < 0. && ds != k)
 804b3f6:	d9 ee                	fldz   
 804b3f8:	dd e9                	fucomp %st(1)
 804b3fa:	df e0                	fnstsw %ax
 804b3fc:	f6 c4 45             	test   $0x45,%ah
 804b3ff:	0f 84 a3 05 00 00    	je     804b9a8 <_dtoa_r+0x7b0>
 804b405:	dd d8                	fstp   %st(0)
    k--;			/* want k = floor(ds) */
  k_check = 1;
  if (k >= 0 && k <= Ten_pmax)
 804b407:	83 7c 24 44 16       	cmpl   $0x16,0x44(%esp)
 804b40c:	0f 87 56 05 00 00    	ja     804b968 <_dtoa_r+0x770>
    {
      if (d.d < tens[k])
 804b412:	8b 44 24 44          	mov    0x44(%esp),%eax
 804b416:	dd 04 c5 20 19 05 08 	fldl   0x8051920(,%eax,8)
 804b41d:	dd 44 24 30          	fldl   0x30(%esp)
 804b421:	d9 c9                	fxch   %st(1)
 804b423:	da e9                	fucompp 
 804b425:	df e0                	fnstsw %ax
 804b427:	f6 c4 45             	test   $0x45,%ah
 804b42a:	0f 85 b8 08 00 00    	jne    804bce8 <_dtoa_r+0xaf0>
	k--;
 804b430:	ff 4c 24 44          	decl   0x44(%esp)
      k_check = 0;
 804b434:	c7 44 24 5c 00 00 00 	movl   $0x0,0x5c(%esp)
 804b43b:	00 
    }
  j = bbits - i - 1;
 804b43c:	29 f3                	sub    %esi,%ebx
  if (j >= 0)
 804b43e:	89 d8                	mov    %ebx,%eax
 804b440:	48                   	dec    %eax
 804b441:	0f 88 4d 05 00 00    	js     804b994 <_dtoa_r+0x79c>
 804b447:	89 44 24 28          	mov    %eax,0x28(%esp)
    {
      b2 = 0;
 804b44b:	c7 44 24 48 00 00 00 	movl   $0x0,0x48(%esp)
 804b452:	00 
  else
    {
      b2 = -j;
      s2 = 0;
    }
  if (k >= 0)
 804b453:	8b 5c 24 44          	mov    0x44(%esp),%ebx
 804b457:	85 db                	test   %ebx,%ebx
 804b459:	0f 88 19 05 00 00    	js     804b978 <_dtoa_r+0x780>
    {
      b5 = 0;
      s5 = k;
      s2 += k;
 804b45f:	8b 44 24 44          	mov    0x44(%esp),%eax
 804b463:	01 44 24 28          	add    %eax,0x28(%esp)
 804b467:	89 44 24 50          	mov    %eax,0x50(%esp)
      b2 = -j;
      s2 = 0;
    }
  if (k >= 0)
    {
      b5 = 0;
 804b46b:	c7 44 24 38 00 00 00 	movl   $0x0,0x38(%esp)
 804b472:	00 
    {
      b2 -= k;
      b5 = -k;
      s5 = 0;
    }
  if (mode < 0 || mode > 9)
 804b473:	83 bc 24 bc 00 00 00 	cmpl   $0x9,0xbc(%esp)
 804b47a:	09 
 804b47b:	0f 87 e7 02 00 00    	ja     804b768 <_dtoa_r+0x570>
    mode = 0;
  try_quick = 1;
  if (mode > 5)
 804b481:	83 bc 24 bc 00 00 00 	cmpl   $0x5,0xbc(%esp)
 804b488:	05 
 804b489:	0f 8e 5d 10 00 00    	jle    804c4ec <_dtoa_r+0x12f4>
    {
      mode -= 4;
 804b48f:	83 ac 24 bc 00 00 00 	subl   $0x4,0xbc(%esp)
 804b496:	04 
      try_quick = 0;
 804b497:	31 ff                	xor    %edi,%edi
    }
  leftright = 1;
  ilim = ilim1 = -1;
  switch (mode)
 804b499:	83 bc 24 bc 00 00 00 	cmpl   $0x3,0xbc(%esp)
 804b4a0:	03 
 804b4a1:	0f 84 89 0c 00 00    	je     804c130 <_dtoa_r+0xf38>
 804b4a7:	0f 8f 5b 09 00 00    	jg     804be08 <_dtoa_r+0xc10>
 804b4ad:	83 bc 24 bc 00 00 00 	cmpl   $0x2,0xbc(%esp)
 804b4b4:	02 
 804b4b5:	0f 85 b8 02 00 00    	jne    804b773 <_dtoa_r+0x57b>
    case 1:
      i = 18;
      ndigits = 0;
      break;
    case 2:
      leftright = 0;
 804b4bb:	c7 44 24 58 00 00 00 	movl   $0x0,0x58(%esp)
 804b4c2:	00 
      /* no break */
    case 4:
      if (ndigits <= 0)
 804b4c3:	8b 8c 24 c0 00 00 00 	mov    0xc0(%esp),%ecx
 804b4ca:	85 c9                	test   %ecx,%ecx
 804b4cc:	0f 8e 9b 0c 00 00    	jle    804c16d <_dtoa_r+0xf75>
 804b4d2:	8b 9c 24 c0 00 00 00 	mov    0xc0(%esp),%ebx
 804b4d9:	89 5c 24 68          	mov    %ebx,0x68(%esp)
 804b4dd:	89 5c 24 40          	mov    %ebx,0x40(%esp)
      ilim1 = i - 1;
      if (i <= 0)
	i = 1;
    }
  j = sizeof (__ULong);
  for (_REENT_MP_RESULT_K(ptr) = 0; sizeof (_Bigint) - sizeof (__ULong) + j <= i;
 804b4e1:	c7 45 44 00 00 00 00 	movl   $0x0,0x44(%ebp)
 804b4e8:	83 fb 17             	cmp    $0x17,%ebx
 804b4eb:	0f 86 05 10 00 00    	jbe    804c4f6 <_dtoa_r+0x12fe>
 804b4f1:	b9 01 00 00 00       	mov    $0x1,%ecx
 804b4f6:	b8 04 00 00 00       	mov    $0x4,%eax
 804b4fb:	90                   	nop
       j <<= 1)
    _REENT_MP_RESULT_K(ptr)++;
 804b4fc:	89 ca                	mov    %ecx,%edx
      if (i <= 0)
	i = 1;
    }
  j = sizeof (__ULong);
  for (_REENT_MP_RESULT_K(ptr) = 0; sizeof (_Bigint) - sizeof (__ULong) + j <= i;
       j <<= 1)
 804b4fe:	d1 e0                	shl    %eax
 804b500:	8d 49 01             	lea    0x1(%ecx),%ecx
      ilim1 = i - 1;
      if (i <= 0)
	i = 1;
    }
  j = sizeof (__ULong);
  for (_REENT_MP_RESULT_K(ptr) = 0; sizeof (_Bigint) - sizeof (__ULong) + j <= i;
 804b503:	8d 70 14             	lea    0x14(%eax),%esi
 804b506:	39 de                	cmp    %ebx,%esi
 804b508:	76 f2                	jbe    804b4fc <_dtoa_r+0x304>
 804b50a:	89 55 44             	mov    %edx,0x44(%ebp)
       j <<= 1)
    _REENT_MP_RESULT_K(ptr)++;
  _REENT_MP_RESULT(ptr) = Balloc (ptr, _REENT_MP_RESULT_K(ptr));
 804b50d:	89 54 24 04          	mov    %edx,0x4(%esp)
 804b511:	89 2c 24             	mov    %ebp,(%esp)
 804b514:	e8 97 28 00 00       	call   804ddb0 <_Balloc>
 804b519:	89 c6                	mov    %eax,%esi
 804b51b:	89 45 40             	mov    %eax,0x40(%ebp)
  s = s0 = (char *) _REENT_MP_RESULT(ptr);

  if (ilim >= 0 && ilim <= Quick_max && try_quick)
 804b51e:	83 fb 0e             	cmp    $0xe,%ebx
 804b521:	0f 87 9d 02 00 00    	ja     804b7c4 <_dtoa_r+0x5cc>
 804b527:	85 ff                	test   %edi,%edi
 804b529:	0f 84 95 02 00 00    	je     804b7c4 <_dtoa_r+0x5cc>
      k0 = k;
      ilim0 = ilim;
      ieps = 2;			/* conservative */
      if (k > 0)
	{
	  ds = tens[k & 0xf];
 804b52f:	8b 44 24 44          	mov    0x44(%esp),%eax
      i = 0;
      d2.d = d.d;
      k0 = k;
      ilim0 = ilim;
      ieps = 2;			/* conservative */
      if (k > 0)
 804b533:	8b 54 24 44          	mov    0x44(%esp),%edx
 804b537:	85 d2                	test   %edx,%edx
 804b539:	0f 8e f5 0c 00 00    	jle    804c234 <_dtoa_r+0x103c>
	{
	  ds = tens[k & 0xf];
 804b53f:	83 e0 0f             	and    $0xf,%eax
 804b542:	dd 04 c5 20 19 05 08 	fldl   0x8051920(,%eax,8)
	  j = k >> 4;
 804b549:	8b 44 24 44          	mov    0x44(%esp),%eax
 804b54d:	c1 f8 04             	sar    $0x4,%eax
	  if (j & Bletch)
 804b550:	a8 10                	test   $0x10,%al
 804b552:	0f 84 07 0c 00 00    	je     804c15f <_dtoa_r+0xf67>
	    {
	      /* prevent overflows */
	      j &= Bletch - 1;
 804b558:	83 e0 0f             	and    $0xf,%eax
	      d.d /= bigtens[n_bigtens - 1];
 804b55b:	dd 44 24 30          	fldl   0x30(%esp)
 804b55f:	dc 35 00 19 05 08    	fdivl  0x8051900
	      ieps++;
 804b565:	ba 03 00 00 00       	mov    $0x3,%edx
	    }
	  for (; j; j >>= 1, i++)
 804b56a:	85 c0                	test   %eax,%eax
 804b56c:	74 17                	je     804b585 <_dtoa_r+0x38d>
 804b56e:	31 c9                	xor    %ecx,%ecx
	    if (j & 1)
 804b570:	a8 01                	test   $0x1,%al
 804b572:	74 0c                	je     804b580 <_dtoa_r+0x388>
 804b574:	d9 c9                	fxch   %st(1)
	      {
		ieps++;
 804b576:	42                   	inc    %edx
		ds *= bigtens[i];
 804b577:	dc 0c cd e0 18 05 08 	fmull  0x80518e0(,%ecx,8)
 804b57e:	d9 c9                	fxch   %st(1)
	      /* prevent overflows */
	      j &= Bletch - 1;
	      d.d /= bigtens[n_bigtens - 1];
	      ieps++;
	    }
	  for (; j; j >>= 1, i++)
 804b580:	41                   	inc    %ecx
 804b581:	d1 f8                	sar    %eax
 804b583:	75 eb                	jne    804b570 <_dtoa_r+0x378>
	    if (j & 1)
	      {
		ieps++;
		ds *= bigtens[i];
	      }
	  d.d /= ds;
 804b585:	de f1                	fdivp  %st,%st(1)
	      {
		ieps++;
		d.d *= bigtens[i];
	      }
	}
      if (k_check && d.d < 1. && ilim > 0)
 804b587:	8b 7c 24 5c          	mov    0x5c(%esp),%edi
 804b58b:	85 ff                	test   %edi,%edi
 804b58d:	74 0f                	je     804b59e <_dtoa_r+0x3a6>
 804b58f:	d9 e8                	fld1   
 804b591:	dd e9                	fucomp %st(1)
 804b593:	df e0                	fnstsw %ax
 804b595:	f6 c4 45             	test   $0x45,%ah
 804b598:	0f 84 78 0e 00 00    	je     804c416 <_dtoa_r+0x121e>
	  ilim = ilim1;
	  k--;
	  d.d *= 10.;
	  ieps++;
	}
      eps.d = ieps * d.d + 7.;
 804b59e:	d9 c0                	fld    %st(0)
 804b5a0:	52                   	push   %edx
 804b5a1:	da 0c 24             	fimull (%esp)
 804b5a4:	58                   	pop    %eax
 804b5a5:	d8 05 70 18 05 08    	fadds  0x8051870
 804b5ab:	dd 5c 24 60          	fstpl  0x60(%esp)
      word0 (eps) -= (P - 1) * Exp_msk1;
 804b5af:	81 6c 24 64 00 00 40 	subl   $0x3400000,0x64(%esp)
 804b5b6:	03 
      if (ilim == 0)
 804b5b7:	8b 44 24 40          	mov    0x40(%esp),%eax
 804b5bb:	85 c0                	test   %eax,%eax
 804b5bd:	0f 84 19 0b 00 00    	je     804c0dc <_dtoa_r+0xee4>
 804b5c3:	8b 5c 24 44          	mov    0x44(%esp),%ebx
 804b5c7:	8b 4c 24 40          	mov    0x40(%esp),%ecx
	  if (d.d < -eps.d)
	    goto no_digits;
	  goto fast_failed;
	}
#ifndef No_leftright
      if (leftright)
 804b5cb:	8b 44 24 58          	mov    0x58(%esp),%eax
 804b5cf:	85 c0                	test   %eax,%eax
 804b5d1:	0f 84 4c 0d 00 00    	je     804c323 <_dtoa_r+0x112b>
	{
	  /* Use Steele & White method of only
	   * generating digits needed.
	   */
	  eps.d = 0.5 / tens[ilim - 1] - eps.d;
 804b5d7:	dd 44 24 60          	fldl   0x60(%esp)
 804b5db:	d9 05 78 18 05 08    	flds   0x8051878
 804b5e1:	dc 34 cd 18 19 05 08 	fdivl  0x8051918(,%ecx,8)
 804b5e8:	de e1                	fsubp  %st,%st(1)
 804b5ea:	d9 c9                	fxch   %st(1)
	  for (i = 0;;)
	    {
	      L = d.d;
 804b5ec:	d9 7c 24 7e          	fnstcw 0x7e(%esp)
 804b5f0:	66 8b 44 24 7e       	mov    0x7e(%esp),%ax
 804b5f5:	b4 0c                	mov    $0xc,%ah
 804b5f7:	66 89 44 24 7c       	mov    %ax,0x7c(%esp)
 804b5fc:	d9 6c 24 7c          	fldcw  0x7c(%esp)
 804b600:	db 54 24 78          	fistl  0x78(%esp)
 804b604:	d9 6c 24 7e          	fldcw  0x7e(%esp)
 804b608:	8b 54 24 78          	mov    0x78(%esp),%edx
	      d.d -= L;
 804b60c:	52                   	push   %edx
 804b60d:	da 24 24             	fisubl (%esp)
 804b610:	d9 c9                	fxch   %st(1)
 804b612:	58                   	pop    %eax
	      *s++ = '0' + (int) L;
 804b613:	83 c2 30             	add    $0x30,%edx
 804b616:	88 16                	mov    %dl,(%esi)
 804b618:	8d 7e 01             	lea    0x1(%esi),%edi
	      if (d.d < eps.d)
 804b61b:	dd e1                	fucom  %st(1)
 804b61d:	df e0                	fnstsw %ax
 804b61f:	f6 c4 45             	test   $0x45,%ah
 804b622:	0f 84 85 00 00 00    	je     804b6ad <_dtoa_r+0x4b5>
		goto ret1;
	      if (1. - d.d < eps.d)
 804b628:	d9 c1                	fld    %st(1)
 804b62a:	d8 2d 68 18 05 08    	fsubrs 0x8051868
 804b630:	d9 c9                	fxch   %st(1)
 804b632:	dd e1                	fucom  %st(1)
 804b634:	df e0                	fnstsw %ax
 804b636:	dd d9                	fstp   %st(1)
 804b638:	f6 c4 45             	test   $0x45,%ah
 804b63b:	0f 84 3d 0e 00 00    	je     804c47e <_dtoa_r+0x1286>
		goto bump_up;
	      if (++i >= ilim)
 804b641:	83 f9 01             	cmp    $0x1,%ecx
 804b644:	0f 8e 6a 01 00 00    	jle    804b7b4 <_dtoa_r+0x5bc>
int _EXFUN(posix_memalign,(void **, size_t, size_t));
#endif

#endif /* ! __STRICT_ANSI__ */

char *	_EXFUN(_dtoa_r,(struct _reent *, double, int, int, int *, int*, char**));
 804b64a:	01 f1                	add    %esi,%ecx
		break;
	      eps.d *= 10.;
 804b64c:	d9 05 6c 18 05 08    	flds   0x805186c
 804b652:	eb 25                	jmp    804b679 <_dtoa_r+0x481>
	      L = d.d;
	      d.d -= L;
	      *s++ = '0' + (int) L;
	      if (d.d < eps.d)
		goto ret1;
	      if (1. - d.d < eps.d)
 804b654:	d9 c1                	fld    %st(1)
 804b656:	d8 2d 68 18 05 08    	fsubrs 0x8051868
 804b65c:	d9 c9                	fxch   %st(1)
 804b65e:	dd e1                	fucom  %st(1)
 804b660:	df e0                	fnstsw %ax
 804b662:	dd d9                	fstp   %st(1)
 804b664:	f6 c4 45             	test   $0x45,%ah
 804b667:	0f 84 17 0e 00 00    	je     804c484 <_dtoa_r+0x128c>
		goto bump_up;
	      if (++i >= ilim)
 804b66d:	39 cf                	cmp    %ecx,%edi
 804b66f:	0f 84 45 01 00 00    	je     804b7ba <_dtoa_r+0x5c2>
 804b675:	d9 c9                	fxch   %st(1)
 804b677:	d9 ca                	fxch   %st(2)
		break;
	      eps.d *= 10.;
 804b679:	dc c9                	fmul   %st,%st(1)
	      d.d *= 10.;
 804b67b:	dc ca                	fmul   %st,%st(2)
 804b67d:	d9 ca                	fxch   %st(2)
	   * generating digits needed.
	   */
	  eps.d = 0.5 / tens[ilim - 1] - eps.d;
	  for (i = 0;;)
	    {
	      L = d.d;
 804b67f:	d9 6c 24 7c          	fldcw  0x7c(%esp)
 804b683:	db 54 24 78          	fistl  0x78(%esp)
 804b687:	d9 6c 24 7e          	fldcw  0x7e(%esp)
 804b68b:	8b 54 24 78          	mov    0x78(%esp),%edx
	      d.d -= L;
 804b68f:	52                   	push   %edx
 804b690:	da 24 24             	fisubl (%esp)
 804b693:	d9 c9                	fxch   %st(1)
 804b695:	58                   	pop    %eax
	      *s++ = '0' + (int) L;
 804b696:	83 c2 30             	add    $0x30,%edx
 804b699:	88 17                	mov    %dl,(%edi)
 804b69b:	47                   	inc    %edi
	      if (d.d < eps.d)
 804b69c:	dd e1                	fucom  %st(1)
 804b69e:	df e0                	fnstsw %ax
 804b6a0:	f6 c4 45             	test   $0x45,%ah
 804b6a3:	75 af                	jne    804b654 <_dtoa_r+0x45c>
 804b6a5:	dd d8                	fstp   %st(0)
 804b6a7:	dd d8                	fstp   %st(0)
 804b6a9:	dd d8                	fstp   %st(0)
 804b6ab:	eb 04                	jmp    804b6b1 <_dtoa_r+0x4b9>
 804b6ad:	dd d8                	fstp   %st(0)
 804b6af:	dd d8                	fstp   %st(0)
 804b6b1:	89 5c 24 44          	mov    %ebx,0x44(%esp)
 804b6b5:	eb 11                	jmp    804b6c8 <_dtoa_r+0x4d0>
 804b6b7:	dd d8                	fstp   %st(0)
 804b6b9:	dd d8                	fstp   %st(0)
 804b6bb:	dd d8                	fstp   %st(0)
 804b6bd:	eb 09                	jmp    804b6c8 <_dtoa_r+0x4d0>
 804b6bf:	90                   	nop
 804b6c0:	dd d8                	fstp   %st(0)
 804b6c2:	dd d8                	fstp   %st(0)
 804b6c4:	dd d8                	fstp   %st(0)
 804b6c6:	66 90                	xchg   %ax,%ax
      if (mlo && mlo != mhi)
	Bfree (ptr, mlo);
      Bfree (ptr, mhi);
    }
ret1:
  Bfree (ptr, b);
 804b6c8:	8b 44 24 54          	mov    0x54(%esp),%eax
 804b6cc:	89 44 24 04          	mov    %eax,0x4(%esp)
 804b6d0:	89 2c 24             	mov    %ebp,(%esp)
 804b6d3:	e8 64 27 00 00       	call   804de3c <_Bfree>
  *s = 0;
 804b6d8:	c6 07 00             	movb   $0x0,(%edi)
  *decpt = k + 1;
 804b6db:	8b 44 24 44          	mov    0x44(%esp),%eax
 804b6df:	40                   	inc    %eax
 804b6e0:	8b 94 24 c4 00 00 00 	mov    0xc4(%esp),%edx
 804b6e7:	89 02                	mov    %eax,(%edx)
  if (rve)
 804b6e9:	8b ac 24 cc 00 00 00 	mov    0xcc(%esp),%ebp
 804b6f0:	85 ed                	test   %ebp,%ebp
 804b6f2:	0f 84 b9 fb ff ff    	je     804b2b1 <_dtoa_r+0xb9>
    *rve = s;
 804b6f8:	8b 84 24 cc 00 00 00 	mov    0xcc(%esp),%eax
 804b6ff:	89 38                	mov    %edi,(%eax)
  return s0;
}
 804b701:	89 f0                	mov    %esi,%eax
 804b703:	81 c4 9c 00 00 00    	add    $0x9c,%esp
 804b709:	5b                   	pop    %ebx
 804b70a:	5e                   	pop    %esi
 804b70b:	5f                   	pop    %edi
 804b70c:	5d                   	pop    %ebp
 804b70d:	c3                   	ret    
 804b70e:	66 90                	xchg   %ax,%ax
    {
      /* Infinity or NaN */
      *decpt = 9999;
      s =
#ifdef IEEE_Arith
	!word1 (d) && !(word0 (d) & 0xfffff) ? "Infinity" :
 804b710:	81 e7 ff ff 0f 00    	and    $0xfffff,%edi
 804b716:	0f 85 bd fb ff ff    	jne    804b2d9 <_dtoa_r+0xe1>
 804b71c:	be 54 18 05 08       	mov    $0x8051854,%esi
 804b721:	e9 b8 fb ff ff       	jmp    804b2de <_dtoa_r+0xe6>
 804b726:	66 90                	xchg   %ax,%ax
#endif
	"NaN";
      if (rve)
	*rve =
#ifdef IEEE_Arith
	  s[3] ? s + 8 :
 804b728:	8d 46 08             	lea    0x8(%esi),%eax
 804b72b:	e9 c6 fb ff ff       	jmp    804b2f6 <_dtoa_r+0xfe>
#else
  if ((i = (int) (word0 (d) >> Exp_shift1 & (Exp_mask >> Exp_shift1))) != 0)
    {
#endif
      d2.d = d.d;
      word0 (d2) &= Frac_mask1;
 804b730:	8b 44 24 2c          	mov    0x2c(%esp),%eax
 804b734:	25 ff ff 0f 00       	and    $0xfffff,%eax
      word0 (d2) |= Exp_11;
 804b739:	0d 00 00 f0 3f       	or     $0x3ff00000,%eax
 804b73e:	89 44 24 2c          	mov    %eax,0x2c(%esp)
		 * Hence we adjust the constant term to 0.1760912590558.
		 * (We could get a more accurate k by invoking log10,
		 *  but this is probably not worthwhile.)
		 */

      i -= Bias;
 804b742:	81 ee ff 03 00 00    	sub    $0x3ff,%esi
 804b748:	8b 9c 24 88 00 00 00 	mov    0x88(%esp),%ebx
#ifdef IBM
      i <<= 2;
      i += j;
#endif
#ifndef Sudden_Underflow
      denorm = 0;
 804b74f:	c7 44 24 4c 00 00 00 	movl   $0x0,0x4c(%esp)
 804b756:	00 
 804b757:	e9 5b fc ff ff       	jmp    804b3b7 <_dtoa_r+0x1bf>
    {
      *decpt = 1;
      s = "0";
      if (rve)
	*rve = s + 1;
      return s;
 804b75c:	be cd 16 05 08       	mov    $0x80516cd,%esi
 804b761:	e9 4b fb ff ff       	jmp    804b2b1 <_dtoa_r+0xb9>
 804b766:	66 90                	xchg   %ax,%ax
      b2 -= k;
      b5 = -k;
      s5 = 0;
    }
  if (mode < 0 || mode > 9)
    mode = 0;
 804b768:	c7 84 24 bc 00 00 00 	movl   $0x0,0xbc(%esp)
 804b76f:	00 00 00 00 
      ilim1 = i - 1;
      if (i <= 0)
	i = 1;
    }
  j = sizeof (__ULong);
  for (_REENT_MP_RESULT_K(ptr) = 0; sizeof (_Bigint) - sizeof (__ULong) + j <= i;
 804b773:	c7 45 44 00 00 00 00 	movl   $0x0,0x44(%ebp)
       j <<= 1)
    _REENT_MP_RESULT_K(ptr)++;
  _REENT_MP_RESULT(ptr) = Balloc (ptr, _REENT_MP_RESULT_K(ptr));
 804b77a:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
 804b781:	00 
 804b782:	89 2c 24             	mov    %ebp,(%esp)
 804b785:	e8 26 26 00 00       	call   804ddb0 <_Balloc>
 804b78a:	89 c6                	mov    %eax,%esi
 804b78c:	89 45 40             	mov    %eax,0x40(%ebp)
 804b78f:	c7 84 24 c0 00 00 00 	movl   $0x0,0xc0(%esp)
 804b796:	00 00 00 00 
 804b79a:	c7 44 24 40 ff ff ff 	movl   $0xffffffff,0x40(%esp)
 804b7a1:	ff 
 804b7a2:	c7 44 24 68 ff ff ff 	movl   $0xffffffff,0x68(%esp)
 804b7a9:	ff 
 804b7aa:	c7 44 24 58 01 00 00 	movl   $0x1,0x58(%esp)
 804b7b1:	00 
 804b7b2:	eb 10                	jmp    804b7c4 <_dtoa_r+0x5cc>
 804b7b4:	dd d8                	fstp   %st(0)
 804b7b6:	dd d8                	fstp   %st(0)
 804b7b8:	eb 0a                	jmp    804b7c4 <_dtoa_r+0x5cc>
 804b7ba:	dd d8                	fstp   %st(0)
 804b7bc:	dd d8                	fstp   %st(0)
 804b7be:	dd d8                	fstp   %st(0)
 804b7c0:	eb 02                	jmp    804b7c4 <_dtoa_r+0x5cc>
 804b7c2:	dd d8                	fstp   %st(0)
      ilim = ilim0;
    }

  /* Do we have a "small" integer? */

  if (be >= 0 && k <= Int_max)
 804b7c4:	8b 84 24 8c 00 00 00 	mov    0x8c(%esp),%eax
 804b7cb:	85 c0                	test   %eax,%eax
 804b7cd:	0f 88 31 01 00 00    	js     804b904 <_dtoa_r+0x70c>
 804b7d3:	83 7c 24 44 0e       	cmpl   $0xe,0x44(%esp)
 804b7d8:	0f 8f 26 01 00 00    	jg     804b904 <_dtoa_r+0x70c>
    {
      /* Yes. */
      ds = tens[k];
 804b7de:	8b 44 24 44          	mov    0x44(%esp),%eax
 804b7e2:	dd 04 c5 20 19 05 08 	fldl   0x8051920(,%eax,8)
      if (ndigits < 0 && ilim <= 0)
 804b7e9:	8b 84 24 c0 00 00 00 	mov    0xc0(%esp),%eax
 804b7f0:	85 c0                	test   %eax,%eax
 804b7f2:	0f 88 9f 06 00 00    	js     804be97 <_dtoa_r+0xc9f>
	    goto no_digits;
	  goto one_digit;
	}
      for (i = 1;; i++)
	{
	  L = d.d / ds;
 804b7f8:	dd 44 24 30          	fldl   0x30(%esp)
 804b7fc:	d9 c0                	fld    %st(0)
 804b7fe:	d8 f2                	fdiv   %st(2),%st
 804b800:	d9 7c 24 7e          	fnstcw 0x7e(%esp)
 804b804:	66 8b 44 24 7e       	mov    0x7e(%esp),%ax
 804b809:	b4 0c                	mov    $0xc,%ah
 804b80b:	66 89 44 24 7c       	mov    %ax,0x7c(%esp)
 804b810:	d9 6c 24 7c          	fldcw  0x7c(%esp)
 804b814:	db 5c 24 78          	fistpl 0x78(%esp)
 804b818:	d9 6c 24 7e          	fldcw  0x7e(%esp)
 804b81c:	8b 4c 24 78          	mov    0x78(%esp),%ecx
	  d.d -= L * ds;
 804b820:	d9 c1                	fld    %st(1)
 804b822:	51                   	push   %ecx
 804b823:	da 0c 24             	fimull (%esp)
 804b826:	58                   	pop    %eax
 804b827:	de e9                	fsubrp %st,%st(1)
	    {
	      L--;
	      d.d += ds;
	    }
#endif
	  *s++ = '0' + (int) L;
 804b829:	8d 41 30             	lea    0x30(%ecx),%eax
 804b82c:	88 06                	mov    %al,(%esi)
 804b82e:	8d 7e 01             	lea    0x1(%esi),%edi
	  if (i == ilim)
 804b831:	83 7c 24 40 01       	cmpl   $0x1,0x40(%esp)
 804b836:	74 70                	je     804b8a8 <_dtoa_r+0x6b0>
		      }
		  ++*s++;
		}
	      break;
	    }
	  if (!(d.d *= 10.))
 804b838:	d9 05 6c 18 05 08    	flds   0x805186c
 804b83e:	dc c9                	fmul   %st,%st(1)
 804b840:	d9 ee                	fldz   
 804b842:	d9 ca                	fxch   %st(2)
 804b844:	dd e2                	fucom  %st(2)
 804b846:	df e0                	fnstsw %ax
 804b848:	dd da                	fstp   %st(2)
 804b84a:	80 e4 45             	and    $0x45,%ah
 804b84d:	80 fc 40             	cmp    $0x40,%ah
 804b850:	0f 84 61 fe ff ff    	je     804b6b7 <_dtoa_r+0x4bf>
 804b856:	8d 56 02             	lea    0x2(%esi),%edx
 804b859:	8b 5c 24 40          	mov    0x40(%esp),%ebx
 804b85d:	01 f3                	add    %esi,%ebx
 804b85f:	eb 1c                	jmp    804b87d <_dtoa_r+0x685>
 804b861:	8d 76 00             	lea    0x0(%esi),%esi
 804b864:	dc c9                	fmul   %st,%st(1)
 804b866:	42                   	inc    %edx
 804b867:	d9 ee                	fldz   
 804b869:	d9 ca                	fxch   %st(2)
 804b86b:	dd e2                	fucom  %st(2)
 804b86d:	df e0                	fnstsw %ax
 804b86f:	dd da                	fstp   %st(2)
 804b871:	80 e4 45             	and    $0x45,%ah
 804b874:	80 fc 40             	cmp    $0x40,%ah
 804b877:	0f 84 43 fe ff ff    	je     804b6c0 <_dtoa_r+0x4c8>
	    goto no_digits;
	  goto one_digit;
	}
      for (i = 1;; i++)
	{
	  L = d.d / ds;
 804b87d:	d9 c1                	fld    %st(1)
 804b87f:	d8 f3                	fdiv   %st(3),%st
 804b881:	d9 6c 24 7c          	fldcw  0x7c(%esp)
 804b885:	db 5c 24 78          	fistpl 0x78(%esp)
 804b889:	d9 6c 24 7e          	fldcw  0x7e(%esp)
 804b88d:	8b 4c 24 78          	mov    0x78(%esp),%ecx
	  d.d -= L * ds;
 804b891:	d9 c2                	fld    %st(2)
 804b893:	51                   	push   %ecx
 804b894:	da 0c 24             	fimull (%esp)
 804b897:	5f                   	pop    %edi
 804b898:	de ea                	fsubrp %st,%st(2)
 804b89a:	89 d7                	mov    %edx,%edi
	    {
	      L--;
	      d.d += ds;
	    }
#endif
	  *s++ = '0' + (int) L;
 804b89c:	8d 41 30             	lea    0x30(%ecx),%eax
 804b89f:	88 42 ff             	mov    %al,-0x1(%edx)
	  if (i == ilim)
 804b8a2:	39 da                	cmp    %ebx,%edx
 804b8a4:	75 be                	jne    804b864 <_dtoa_r+0x66c>
 804b8a6:	dd d8                	fstp   %st(0)
	    {
	      d.d += d.d;
 804b8a8:	d8 c0                	fadd   %st(0),%st
             if ((d.d > ds) || ((d.d == ds) && (L & 1)))
 804b8aa:	dd e1                	fucom  %st(1)
 804b8ac:	df e0                	fnstsw %ax
 804b8ae:	f6 c4 45             	test   $0x45,%ah
 804b8b1:	74 1d                	je     804b8d0 <_dtoa_r+0x6d8>
 804b8b3:	d9 c9                	fxch   %st(1)
 804b8b5:	da e9                	fucompp 
 804b8b7:	df e0                	fnstsw %ax
 804b8b9:	80 e4 45             	and    $0x45,%ah
 804b8bc:	80 f4 40             	xor    $0x40,%ah
 804b8bf:	0f 85 03 fe ff ff    	jne    804b6c8 <_dtoa_r+0x4d0>
 804b8c5:	83 e1 01             	and    $0x1,%ecx
 804b8c8:	0f 84 fa fd ff ff    	je     804b6c8 <_dtoa_r+0x4d0>
 804b8ce:	eb 04                	jmp    804b8d4 <_dtoa_r+0x6dc>
 804b8d0:	dd d8                	fstp   %st(0)
 804b8d2:	dd d8                	fstp   %st(0)
 804b8d4:	8a 57 ff             	mov    -0x1(%edi),%dl
 804b8d7:	eb 08                	jmp    804b8e1 <_dtoa_r+0x6e9>
 804b8d9:	8d 76 00             	lea    0x0(%esi),%esi
 804b8dc:	8a 50 ff             	mov    -0x1(%eax),%dl
 804b8df:	89 c7                	mov    %eax,%edi
		{
		bump_up:
		  while (*--s == '9')
 804b8e1:	8d 47 ff             	lea    -0x1(%edi),%eax
 804b8e4:	80 fa 39             	cmp    $0x39,%dl
 804b8e7:	0f 85 fa 0a 00 00    	jne    804c3e7 <_dtoa_r+0x11ef>
		    if (s == s0)
 804b8ed:	39 c6                	cmp    %eax,%esi
 804b8ef:	75 eb                	jne    804b8dc <_dtoa_r+0x6e4>
		      {
			k++;
 804b8f1:	ff 44 24 44          	incl   0x44(%esp)
			*s = '0';
 804b8f5:	c6 06 30             	movb   $0x30,(%esi)
			break;
 804b8f8:	89 f0                	mov    %esi,%eax
 804b8fa:	b2 31                	mov    $0x31,%dl
		      }
		  ++*s++;
 804b8fc:	88 10                	mov    %dl,(%eax)
 804b8fe:	e9 c5 fd ff ff       	jmp    804b6c8 <_dtoa_r+0x4d0>
 804b903:	90                   	nop
    }

  m2 = b2;
  m5 = b5;
  mhi = mlo = 0;
  if (leftright)
 804b904:	8b 4c 24 58          	mov    0x58(%esp),%ecx
 804b908:	85 c9                	test   %ecx,%ecx
 804b90a:	0f 84 b8 00 00 00    	je     804b9c8 <_dtoa_r+0x7d0>
    {
      if (mode < 2)
 804b910:	83 bc 24 bc 00 00 00 	cmpl   $0x1,0xbc(%esp)
 804b917:	01 
 804b918:	0f 8e ff 05 00 00    	jle    804bf1d <_dtoa_r+0xd25>
	    1 + P - bbits;
#endif
	}
      else
	{
	  j = ilim - 1;
 804b91e:	8b 44 24 40          	mov    0x40(%esp),%eax
 804b922:	48                   	dec    %eax
	  if (m5 >= j)
 804b923:	39 44 24 38          	cmp    %eax,0x38(%esp)
 804b927:	0f 8c 1d 08 00 00    	jl     804c14a <_dtoa_r+0xf52>
	    m5 -= j;
 804b92d:	8b 7c 24 38          	mov    0x38(%esp),%edi
 804b931:	29 c7                	sub    %eax,%edi
	      b5 += j;
	      m5 = 0;
	    }
	  if ((i = ilim) < 0)
	    {
	      m2 -= i;
 804b933:	8b 5c 24 48          	mov    0x48(%esp),%ebx
	    {
	      s5 += j -= m5;
	      b5 += j;
	      m5 = 0;
	    }
	  if ((i = ilim) < 0)
 804b937:	8b 44 24 40          	mov    0x40(%esp),%eax
 804b93b:	85 c0                	test   %eax,%eax
 804b93d:	0f 88 8f 0a 00 00    	js     804c3d2 <_dtoa_r+0x11da>
 804b943:	8b 44 24 40          	mov    0x40(%esp),%eax
	    {
	      m2 -= i;
	      i = 0;
	    }
	}
      b2 += i;
 804b947:	01 44 24 48          	add    %eax,0x48(%esp)
      s2 += i;
 804b94b:	01 44 24 28          	add    %eax,0x28(%esp)
      mhi = i2b (ptr, 1);
 804b94f:	c7 44 24 04 01 00 00 	movl   $0x1,0x4(%esp)
 804b956:	00 
 804b957:	89 2c 24             	mov    %ebp,(%esp)
 804b95a:	e8 a1 27 00 00       	call   804e100 <__i2b>
 804b95f:	89 44 24 4c          	mov    %eax,0x4c(%esp)
 804b963:	eb 73                	jmp    804b9d8 <_dtoa_r+0x7e0>
 804b965:	8d 76 00             	lea    0x0(%esi),%esi
  ds = (d2.d - 1.5) * 0.289529654602168 + 0.1760912590558 + i * 0.301029995663981;
#endif
  k = (int) ds;
  if (ds < 0. && ds != k)
    k--;			/* want k = floor(ds) */
  k_check = 1;
 804b968:	c7 44 24 5c 01 00 00 	movl   $0x1,0x5c(%esp)
 804b96f:	00 
 804b970:	e9 c7 fa ff ff       	jmp    804b43c <_dtoa_r+0x244>
 804b975:	8d 76 00             	lea    0x0(%esi),%esi
      s5 = k;
      s2 += k;
    }
  else
    {
      b2 -= k;
 804b978:	8b 54 24 44          	mov    0x44(%esp),%edx
 804b97c:	29 54 24 48          	sub    %edx,0x48(%esp)
      b5 = -k;
 804b980:	f7 da                	neg    %edx
 804b982:	89 54 24 38          	mov    %edx,0x38(%esp)
      s5 = 0;
 804b986:	c7 44 24 50 00 00 00 	movl   $0x0,0x50(%esp)
 804b98d:	00 
 804b98e:	e9 e0 fa ff ff       	jmp    804b473 <_dtoa_r+0x27b>
 804b993:	90                   	nop
      b2 = 0;
      s2 = j;
    }
  else
    {
      b2 = -j;
 804b994:	f7 d8                	neg    %eax
 804b996:	89 44 24 48          	mov    %eax,0x48(%esp)
      s2 = 0;
 804b99a:	c7 44 24 28 00 00 00 	movl   $0x0,0x28(%esp)
 804b9a1:	00 
 804b9a2:	e9 ac fa ff ff       	jmp    804b453 <_dtoa_r+0x25b>
 804b9a7:	90                   	nop
  ds = (d2.d - 1.5) * 0.289529651 + 0.176091269 + i * 0.30103001;
#else
  ds = (d2.d - 1.5) * 0.289529654602168 + 0.1760912590558 + i * 0.301029995663981;
#endif
  k = (int) ds;
  if (ds < 0. && ds != k)
 804b9a8:	db 44 24 44          	fildl  0x44(%esp)
 804b9ac:	da e9                	fucompp 
 804b9ae:	df e0                	fnstsw %ax
 804b9b0:	80 e4 45             	and    $0x45,%ah
 804b9b3:	80 fc 40             	cmp    $0x40,%ah
 804b9b6:	0f 84 4b fa ff ff    	je     804b407 <_dtoa_r+0x20f>
    k--;			/* want k = floor(ds) */
 804b9bc:	ff 4c 24 44          	decl   0x44(%esp)
 804b9c0:	e9 42 fa ff ff       	jmp    804b407 <_dtoa_r+0x20f>
 804b9c5:	8d 76 00             	lea    0x0(%esi),%esi
    }

  m2 = b2;
  m5 = b5;
  mhi = mlo = 0;
  if (leftright)
 804b9c8:	8b 7c 24 38          	mov    0x38(%esp),%edi
	    break;
	}
      goto ret1;
    }

  m2 = b2;
 804b9cc:	8b 5c 24 48          	mov    0x48(%esp),%ebx
  m5 = b5;
  mhi = mlo = 0;
 804b9d0:	c7 44 24 4c 00 00 00 	movl   $0x0,0x4c(%esp)
 804b9d7:	00 
	}
      b2 += i;
      s2 += i;
      mhi = i2b (ptr, 1);
    }
  if (m2 > 0 && s2 > 0)
 804b9d8:	85 db                	test   %ebx,%ebx
 804b9da:	74 1e                	je     804b9fa <_dtoa_r+0x802>
 804b9dc:	8b 44 24 28          	mov    0x28(%esp),%eax
 804b9e0:	85 c0                	test   %eax,%eax
 804b9e2:	7e 16                	jle    804b9fa <_dtoa_r+0x802>
    {
      i = m2 < s2 ? m2 : s2;
 804b9e4:	8b 44 24 28          	mov    0x28(%esp),%eax
 804b9e8:	39 d8                	cmp    %ebx,%eax
 804b9ea:	0f 8f a0 04 00 00    	jg     804be90 <_dtoa_r+0xc98>
      b2 -= i;
 804b9f0:	29 44 24 48          	sub    %eax,0x48(%esp)
      m2 -= i;
 804b9f4:	29 c3                	sub    %eax,%ebx
      s2 -= i;
 804b9f6:	29 44 24 28          	sub    %eax,0x28(%esp)
    }
  if (b5 > 0)
 804b9fa:	8b 44 24 38          	mov    0x38(%esp),%eax
 804b9fe:	85 c0                	test   %eax,%eax
 804ba00:	7e 68                	jle    804ba6a <_dtoa_r+0x872>
    {
      if (leftright)
 804ba02:	8b 4c 24 58          	mov    0x58(%esp),%ecx
 804ba06:	85 c9                	test   %ecx,%ecx
 804ba08:	0f 84 12 07 00 00    	je     804c120 <_dtoa_r+0xf28>
	{
	  if (m5 > 0)
 804ba0e:	85 ff                	test   %edi,%edi
 804ba10:	7e 4c                	jle    804ba5e <_dtoa_r+0x866>
	    {
	      mhi = pow5mult (ptr, mhi, m5);
 804ba12:	89 7c 24 08          	mov    %edi,0x8(%esp)
 804ba16:	8b 44 24 4c          	mov    0x4c(%esp),%eax
 804ba1a:	89 44 24 04          	mov    %eax,0x4(%esp)
 804ba1e:	89 2c 24             	mov    %ebp,(%esp)
 804ba21:	e8 c6 28 00 00       	call   804e2ec <__pow5mult>
 804ba26:	89 44 24 4c          	mov    %eax,0x4c(%esp)
	      b1 = mult (ptr, mhi, b);
 804ba2a:	8b 44 24 54          	mov    0x54(%esp),%eax
 804ba2e:	89 44 24 08          	mov    %eax,0x8(%esp)
 804ba32:	8b 44 24 4c          	mov    0x4c(%esp),%eax
 804ba36:	89 44 24 04          	mov    %eax,0x4(%esp)
 804ba3a:	89 2c 24             	mov    %ebp,(%esp)
 804ba3d:	e8 ea 26 00 00       	call   804e12c <__multiply>
	      Bfree (ptr, b);
 804ba42:	8b 54 24 54          	mov    0x54(%esp),%edx
 804ba46:	89 54 24 04          	mov    %edx,0x4(%esp)
 804ba4a:	89 2c 24             	mov    %ebp,(%esp)
 804ba4d:	89 44 24 24          	mov    %eax,0x24(%esp)
 804ba51:	e8 e6 23 00 00       	call   804de3c <_Bfree>
	      b = b1;
 804ba56:	8b 44 24 24          	mov    0x24(%esp),%eax
 804ba5a:	89 44 24 54          	mov    %eax,0x54(%esp)
	    }
         if ((j = b5 - m5) != 0)
 804ba5e:	8b 44 24 38          	mov    0x38(%esp),%eax
 804ba62:	29 f8                	sub    %edi,%eax
 804ba64:	0f 85 96 04 00 00    	jne    804bf00 <_dtoa_r+0xd08>
	    b = pow5mult (ptr, b, j);
	}
      else
	b = pow5mult (ptr, b, b5);
    }
  S = i2b (ptr, 1);
 804ba6a:	c7 44 24 04 01 00 00 	movl   $0x1,0x4(%esp)
 804ba71:	00 
 804ba72:	89 2c 24             	mov    %ebp,(%esp)
 804ba75:	e8 86 26 00 00       	call   804e100 <__i2b>
 804ba7a:	89 44 24 38          	mov    %eax,0x38(%esp)
  if (s5 > 0)
 804ba7e:	8b 54 24 50          	mov    0x50(%esp),%edx
 804ba82:	85 d2                	test   %edx,%edx
 804ba84:	7e 18                	jle    804ba9e <_dtoa_r+0x8a6>
    S = pow5mult (ptr, S, s5);
 804ba86:	8b 54 24 50          	mov    0x50(%esp),%edx
 804ba8a:	89 54 24 08          	mov    %edx,0x8(%esp)
 804ba8e:	89 44 24 04          	mov    %eax,0x4(%esp)
 804ba92:	89 2c 24             	mov    %ebp,(%esp)
 804ba95:	e8 52 28 00 00       	call   804e2ec <__pow5mult>
 804ba9a:	89 44 24 38          	mov    %eax,0x38(%esp)

  /* Check for special case that d is a normalized power of 2. */

  spec_case = 0;
  if (mode < 2)
 804ba9e:	83 bc 24 bc 00 00 00 	cmpl   $0x1,0xbc(%esp)
 804baa5:	01 
 804baa6:	0f 8e ac 03 00 00    	jle    804be58 <_dtoa_r+0xc60>
  if (s5 > 0)
    S = pow5mult (ptr, S, s5);

  /* Check for special case that d is a normalized power of 2. */

  spec_case = 0;
 804baac:	31 ff                	xor    %edi,%edi
   * and for all and pass them and a shift to quorem, so it
   * can do shifts and ors to compute the numerator for q.
   */

#ifdef Pack_32
  if ((i = ((s5 ? 32 - hi0bits (S->_x[S->_wds - 1]) : 1) + s2) & 0x1f) != 0)
 804baae:	8b 44 24 50          	mov    0x50(%esp),%eax
 804bab2:	85 c0                	test   %eax,%eax
 804bab4:	0f 85 22 05 00 00    	jne    804bfdc <_dtoa_r+0xde4>
 804baba:	ba 01 00 00 00       	mov    $0x1,%edx
 804babf:	8b 44 24 28          	mov    0x28(%esp),%eax
 804bac3:	01 d0                	add    %edx,%eax
 804bac5:	83 e0 1f             	and    $0x1f,%eax
 804bac8:	0f 84 f2 01 00 00    	je     804bcc0 <_dtoa_r+0xac8>
    i = 32 - i;
 804bace:	ba 20 00 00 00       	mov    $0x20,%edx
 804bad3:	29 c2                	sub    %eax,%edx
#else
  if ((i = ((s5 ? 32 - hi0bits (S->_x[S->_wds - 1]) : 1) + s2) & 0xf) != 0)
    i = 16 - i;
#endif
  if (i > 4)
 804bad5:	83 fa 04             	cmp    $0x4,%edx
 804bad8:	0f 8e fc 09 00 00    	jle    804c4da <_dtoa_r+0x12e2>
    {
      i -= 4;
 804bade:	ba 1c 00 00 00       	mov    $0x1c,%edx
 804bae3:	29 c2                	sub    %eax,%edx
      b2 += i;
 804bae5:	01 54 24 48          	add    %edx,0x48(%esp)
      m2 += i;
 804bae9:	01 d3                	add    %edx,%ebx
      s2 += i;
 804baeb:	01 54 24 28          	add    %edx,0x28(%esp)
      i += 28;
      b2 += i;
      m2 += i;
      s2 += i;
    }
  if (b2 > 0)
 804baef:	8b 4c 24 48          	mov    0x48(%esp),%ecx
 804baf3:	85 c9                	test   %ecx,%ecx
 804baf5:	7e 1c                	jle    804bb13 <_dtoa_r+0x91b>
    b = lshift (ptr, b, b2);
 804baf7:	8b 54 24 48          	mov    0x48(%esp),%edx
 804bafb:	89 54 24 08          	mov    %edx,0x8(%esp)
 804baff:	8b 44 24 54          	mov    0x54(%esp),%eax
 804bb03:	89 44 24 04          	mov    %eax,0x4(%esp)
 804bb07:	89 2c 24             	mov    %ebp,(%esp)
 804bb0a:	e8 b9 28 00 00       	call   804e3c8 <__lshift>
 804bb0f:	89 44 24 54          	mov    %eax,0x54(%esp)
  if (s2 > 0)
 804bb13:	8b 54 24 28          	mov    0x28(%esp),%edx
 804bb17:	85 d2                	test   %edx,%edx
 804bb19:	7e 1c                	jle    804bb37 <_dtoa_r+0x93f>
    S = lshift (ptr, S, s2);
 804bb1b:	8b 54 24 28          	mov    0x28(%esp),%edx
 804bb1f:	89 54 24 08          	mov    %edx,0x8(%esp)
 804bb23:	8b 44 24 38          	mov    0x38(%esp),%eax
 804bb27:	89 44 24 04          	mov    %eax,0x4(%esp)
 804bb2b:	89 2c 24             	mov    %ebp,(%esp)
 804bb2e:	e8 95 28 00 00       	call   804e3c8 <__lshift>
 804bb33:	89 44 24 38          	mov    %eax,0x38(%esp)
  if (k_check)
 804bb37:	8b 44 24 5c          	mov    0x5c(%esp),%eax
 804bb3b:	85 c0                	test   %eax,%eax
 804bb3d:	0f 85 b9 04 00 00    	jne    804bffc <_dtoa_r+0xe04>
	  if (leftright)
	    mhi = multadd (ptr, mhi, 10, 0);
	  ilim = ilim1;
	}
    }
  if (ilim <= 0 && mode > 2)
 804bb43:	8b 44 24 40          	mov    0x40(%esp),%eax
 804bb47:	85 c0                	test   %eax,%eax
 804bb49:	0f 8e 0d 05 00 00    	jle    804c05c <_dtoa_r+0xe64>
    one_digit:
      *s++ = '1';
      k++;
      goto ret;
    }
  if (leftright)
 804bb4f:	8b 44 24 58          	mov    0x58(%esp),%eax
 804bb53:	85 c0                	test   %eax,%eax
 804bb55:	0f 84 9d 01 00 00    	je     804bcf8 <_dtoa_r+0xb00>
    {
      if (m2 > 0)
 804bb5b:	85 db                	test   %ebx,%ebx
 804bb5d:	7e 18                	jle    804bb77 <_dtoa_r+0x97f>
	mhi = lshift (ptr, mhi, m2);
 804bb5f:	89 5c 24 08          	mov    %ebx,0x8(%esp)
 804bb63:	8b 44 24 4c          	mov    0x4c(%esp),%eax
 804bb67:	89 44 24 04          	mov    %eax,0x4(%esp)
 804bb6b:	89 2c 24             	mov    %ebp,(%esp)
 804bb6e:	e8 55 28 00 00       	call   804e3c8 <__lshift>
 804bb73:	89 44 24 4c          	mov    %eax,0x4c(%esp)
      /* Compute mlo -- check for special case
       * that d is a normalized power of 2.
       */

      mlo = mhi;
      if (spec_case)
 804bb77:	85 ff                	test   %edi,%edi
 804bb79:	0f 85 fb 06 00 00    	jne    804c27a <_dtoa_r+0x1082>
 804bb7f:	8b 44 24 4c          	mov    0x4c(%esp),%eax
 804bb83:	89 44 24 28          	mov    %eax,0x28(%esp)
 804bb87:	8d 5e 01             	lea    0x1(%esi),%ebx
 804bb8a:	8b 54 24 40          	mov    0x40(%esp),%edx
 804bb8e:	01 f2                	add    %esi,%edx
 804bb90:	89 54 24 50          	mov    %edx,0x50(%esp)
 804bb94:	8b 4c 24 4c          	mov    0x4c(%esp),%ecx
	  j = cmp (b, mlo);
	  delta = diff (ptr, S, mhi);
	  j1 = delta->_sign ? 1 : cmp (b, delta);
	  Bfree (ptr, delta);
#ifndef ROUND_BIASED
	  if (j1 == 0 && !mode && !(word1 (d) & 1))
 804bb98:	8b 44 24 30          	mov    0x30(%esp),%eax
 804bb9c:	83 e0 01             	and    $0x1,%eax
 804bb9f:	89 44 24 4c          	mov    %eax,0x4c(%esp)
 804bba3:	89 cf                	mov    %ecx,%edi
 804bba5:	89 74 24 58          	mov    %esi,0x58(%esp)
 804bba9:	8b 74 24 54          	mov    0x54(%esp),%esi
 804bbad:	8d 76 00             	lea    0x0(%esi),%esi
 804bbb0:	8d 53 ff             	lea    -0x1(%ebx),%edx
 804bbb3:	89 54 24 48          	mov    %edx,0x48(%esp)
	  mhi = lshift (ptr, mhi, Log2P);
	}

      for (i = 1;; i++)
	{
	  dig = quorem (b, S) + '0';
 804bbb7:	8b 54 24 38          	mov    0x38(%esp),%edx
 804bbbb:	89 f0                	mov    %esi,%eax
 804bbbd:	e8 56 f4 ff ff       	call   804b018 <quorem>
 804bbc2:	89 44 24 3c          	mov    %eax,0x3c(%esp)
 804bbc6:	83 c0 30             	add    $0x30,%eax
 804bbc9:	89 44 24 30          	mov    %eax,0x30(%esp)
	  /* Do we yet have the shortest decimal string
	   * that will round to d?
	   */
	  j = cmp (b, mlo);
 804bbcd:	89 7c 24 04          	mov    %edi,0x4(%esp)
 804bbd1:	89 34 24             	mov    %esi,(%esp)
 804bbd4:	e8 e7 28 00 00       	call   804e4c0 <__mcmp>
 804bbd9:	89 44 24 40          	mov    %eax,0x40(%esp)
	  delta = diff (ptr, S, mhi);
 804bbdd:	8b 44 24 28          	mov    0x28(%esp),%eax
 804bbe1:	89 44 24 08          	mov    %eax,0x8(%esp)
 804bbe5:	8b 44 24 38          	mov    0x38(%esp),%eax
 804bbe9:	89 44 24 04          	mov    %eax,0x4(%esp)
 804bbed:	89 2c 24             	mov    %ebp,(%esp)
 804bbf0:	e8 13 29 00 00       	call   804e508 <__mdiff>
 804bbf5:	89 c2                	mov    %eax,%edx
	  j1 = delta->_sign ? 1 : cmp (b, delta);
 804bbf7:	8b 40 0c             	mov    0xc(%eax),%eax
 804bbfa:	85 c0                	test   %eax,%eax
 804bbfc:	89 54 24 04          	mov    %edx,0x4(%esp)
 804bc00:	0f 84 36 03 00 00    	je     804bf3c <_dtoa_r+0xd44>
	  Bfree (ptr, delta);
 804bc06:	89 2c 24             	mov    %ebp,(%esp)
 804bc09:	e8 2e 22 00 00       	call   804de3c <_Bfree>
	  /* Do we yet have the shortest decimal string
	   * that will round to d?
	   */
	  j = cmp (b, mlo);
	  delta = diff (ptr, S, mhi);
	  j1 = delta->_sign ? 1 : cmp (b, delta);
 804bc0e:	b8 01 00 00 00       	mov    $0x1,%eax
		dig++;
	      *s++ = dig;
	      goto ret;
	    }
#endif
         if ((j < 0) || ((j == 0) && !mode
 804bc13:	83 7c 24 40 00       	cmpl   $0x0,0x40(%esp)
 804bc18:	0f 8c 74 05 00 00    	jl     804c192 <_dtoa_r+0xf9a>
 804bc1e:	75 17                	jne    804bc37 <_dtoa_r+0xa3f>
 804bc20:	8b 8c 24 bc 00 00 00 	mov    0xbc(%esp),%ecx
 804bc27:	85 c9                	test   %ecx,%ecx
 804bc29:	75 0c                	jne    804bc37 <_dtoa_r+0xa3f>
#ifndef ROUND_BIASED
	      && !(word1 (d) & 1)
 804bc2b:	8b 54 24 4c          	mov    0x4c(%esp),%edx
 804bc2f:	85 d2                	test   %edx,%edx
 804bc31:	0f 84 5b 05 00 00    	je     804c192 <_dtoa_r+0xf9a>
		    goto round_9_up;
		}
	      *s++ = dig;
	      goto ret;
	    }
	  if (j1 > 0)
 804bc37:	85 c0                	test   %eax,%eax
 804bc39:	0f 8f 93 06 00 00    	jg     804c2d2 <_dtoa_r+0x10da>
		  goto roundoff;
		}
	      *s++ = dig + 1;
	      goto ret;
	    }
	  *s++ = dig;
 804bc3f:	8a 54 24 30          	mov    0x30(%esp),%dl
 804bc43:	88 53 ff             	mov    %dl,-0x1(%ebx)
	  if (i == ilim)
 804bc46:	3b 5c 24 50          	cmp    0x50(%esp),%ebx
 804bc4a:	0f 84 a1 06 00 00    	je     804c2f1 <_dtoa_r+0x10f9>
	    break;
	  b = multadd (ptr, b, 10, 0);
 804bc50:	c7 44 24 0c 00 00 00 	movl   $0x0,0xc(%esp)
 804bc57:	00 
 804bc58:	c7 44 24 08 0a 00 00 	movl   $0xa,0x8(%esp)
 804bc5f:	00 
 804bc60:	89 74 24 04          	mov    %esi,0x4(%esp)
 804bc64:	89 2c 24             	mov    %ebp,(%esp)
 804bc67:	e8 ec 21 00 00       	call   804de58 <__multadd>
 804bc6c:	89 c6                	mov    %eax,%esi
	  if (mlo == mhi)
 804bc6e:	3b 7c 24 28          	cmp    0x28(%esp),%edi
	    mlo = mhi = multadd (ptr, mhi, 10, 0);
 804bc72:	c7 44 24 0c 00 00 00 	movl   $0x0,0xc(%esp)
 804bc79:	00 
 804bc7a:	c7 44 24 08 0a 00 00 	movl   $0xa,0x8(%esp)
 804bc81:	00 
 804bc82:	89 7c 24 04          	mov    %edi,0x4(%esp)
 804bc86:	89 2c 24             	mov    %ebp,(%esp)
	    }
	  *s++ = dig;
	  if (i == ilim)
	    break;
	  b = multadd (ptr, b, 10, 0);
	  if (mlo == mhi)
 804bc89:	0f 84 39 03 00 00    	je     804bfc8 <_dtoa_r+0xdd0>
	    mlo = mhi = multadd (ptr, mhi, 10, 0);
	  else
	    {
	      mlo = multadd (ptr, mlo, 10, 0);
 804bc8f:	e8 c4 21 00 00       	call   804de58 <__multadd>
 804bc94:	89 c7                	mov    %eax,%edi
	      mhi = multadd (ptr, mhi, 10, 0);
 804bc96:	c7 44 24 0c 00 00 00 	movl   $0x0,0xc(%esp)
 804bc9d:	00 
 804bc9e:	c7 44 24 08 0a 00 00 	movl   $0xa,0x8(%esp)
 804bca5:	00 
 804bca6:	8b 44 24 28          	mov    0x28(%esp),%eax
 804bcaa:	89 44 24 04          	mov    %eax,0x4(%esp)
 804bcae:	89 2c 24             	mov    %ebp,(%esp)
 804bcb1:	e8 a2 21 00 00       	call   804de58 <__multadd>
 804bcb6:	89 44 24 28          	mov    %eax,0x28(%esp)
 804bcba:	43                   	inc    %ebx
	    }
	}
 804bcbb:	e9 f0 fe ff ff       	jmp    804bbb0 <_dtoa_r+0x9b8>
   * and for all and pass them and a shift to quorem, so it
   * can do shifts and ors to compute the numerator for q.
   */

#ifdef Pack_32
  if ((i = ((s5 ? 32 - hi0bits (S->_x[S->_wds - 1]) : 1) + s2) & 0x1f) != 0)
 804bcc0:	ba 1c 00 00 00       	mov    $0x1c,%edx
      s2 += i;
    }
  else if (i < 4)
    {
      i += 28;
      b2 += i;
 804bcc5:	01 54 24 48          	add    %edx,0x48(%esp)
      m2 += i;
 804bcc9:	01 d3                	add    %edx,%ebx
      s2 += i;
 804bccb:	01 54 24 28          	add    %edx,0x28(%esp)
 804bccf:	e9 1b fe ff ff       	jmp    804baef <_dtoa_r+0x8f7>
      i = bbits + be + (Bias + (P - 1) - 1);
#if defined (_DOUBLE_IS_32BITS)
      x = word0 (d) << (32 - i);
#else
      x = (i > 32) ? (word0 (d) << (64 - i)) | (word1 (d) >> (i - 32))
       : (word1 (d) << (32 - i));
 804bcd4:	b9 ee fb ff ff       	mov    $0xfffffbee,%ecx
 804bcd9:	29 f1                	sub    %esi,%ecx

      i = bbits + be + (Bias + (P - 1) - 1);
#if defined (_DOUBLE_IS_32BITS)
      x = word0 (d) << (32 - i);
#else
      x = (i > 32) ? (word0 (d) << (64 - i)) | (word1 (d) >> (i - 32))
 804bcdb:	8b 44 24 30          	mov    0x30(%esp),%eax
 804bcdf:	d3 e0                	shl    %cl,%eax
 804bce1:	e9 ae f6 ff ff       	jmp    804b394 <_dtoa_r+0x19c>
 804bce6:	66 90                	xchg   %ax,%ax
  k_check = 1;
  if (k >= 0 && k <= Ten_pmax)
    {
      if (d.d < tens[k])
	k--;
      k_check = 0;
 804bce8:	c7 44 24 5c 00 00 00 	movl   $0x0,0x5c(%esp)
 804bcef:	00 
 804bcf0:	e9 47 f7 ff ff       	jmp    804b43c <_dtoa_r+0x244>
 804bcf5:	8d 76 00             	lea    0x0(%esi),%esi
    one_digit:
      *s++ = '1';
      k++;
      goto ret;
    }
  if (leftright)
 804bcf8:	bb 01 00 00 00       	mov    $0x1,%ebx
 804bcfd:	8b 7c 24 54          	mov    0x54(%esp),%edi
 804bd01:	eb 20                	jmp    804bd23 <_dtoa_r+0xb2b>
 804bd03:	90                   	nop
    for (i = 1;; i++)
      {
	*s++ = dig = quorem (b, S) + '0';
	if (i >= ilim)
	  break;
	b = multadd (ptr, b, 10, 0);
 804bd04:	c7 44 24 0c 00 00 00 	movl   $0x0,0xc(%esp)
 804bd0b:	00 
 804bd0c:	c7 44 24 08 0a 00 00 	movl   $0xa,0x8(%esp)
 804bd13:	00 
 804bd14:	89 7c 24 04          	mov    %edi,0x4(%esp)
 804bd18:	89 2c 24             	mov    %ebp,(%esp)
 804bd1b:	e8 38 21 00 00       	call   804de58 <__multadd>
 804bd20:	89 c7                	mov    %eax,%edi
	      mhi = multadd (ptr, mhi, 10, 0);
	    }
	}
    }
  else
    for (i = 1;; i++)
 804bd22:	43                   	inc    %ebx
      {
	*s++ = dig = quorem (b, S) + '0';
 804bd23:	8b 54 24 38          	mov    0x38(%esp),%edx
 804bd27:	89 f8                	mov    %edi,%eax
 804bd29:	e8 ea f2 ff ff       	call   804b018 <quorem>
 804bd2e:	83 c0 30             	add    $0x30,%eax
 804bd31:	88 44 1e ff          	mov    %al,-0x1(%esi,%ebx,1)
	if (i >= ilim)
 804bd35:	3b 5c 24 40          	cmp    0x40(%esp),%ebx
 804bd39:	7c c9                	jl     804bd04 <_dtoa_r+0xb0c>
 804bd3b:	89 44 24 30          	mov    %eax,0x30(%esp)
 804bd3f:	89 7c 24 54          	mov    %edi,0x54(%esp)
 804bd43:	8b 5c 24 40          	mov    0x40(%esp),%ebx
 804bd47:	85 db                	test   %ebx,%ebx
 804bd49:	0f 8e 8e 06 00 00    	jle    804c3dd <_dtoa_r+0x11e5>
 804bd4f:	8d 3c 1e             	lea    (%esi,%ebx,1),%edi
      goto ret1;
    }

  m2 = b2;
  m5 = b5;
  mhi = mlo = 0;
 804bd52:	31 c9                	xor    %ecx,%ecx
	b = multadd (ptr, b, 10, 0);
      }

  /* Round off last digit */

  b = lshift (ptr, b, 1);
 804bd54:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
 804bd5b:	00 
 804bd5c:	8b 44 24 54          	mov    0x54(%esp),%eax
 804bd60:	89 44 24 04          	mov    %eax,0x4(%esp)
 804bd64:	89 2c 24             	mov    %ebp,(%esp)
 804bd67:	89 4c 24 24          	mov    %ecx,0x24(%esp)
 804bd6b:	e8 58 26 00 00       	call   804e3c8 <__lshift>
 804bd70:	89 44 24 54          	mov    %eax,0x54(%esp)
  j = cmp (b, S);
 804bd74:	8b 44 24 38          	mov    0x38(%esp),%eax
 804bd78:	89 44 24 04          	mov    %eax,0x4(%esp)
 804bd7c:	8b 44 24 54          	mov    0x54(%esp),%eax
 804bd80:	89 04 24             	mov    %eax,(%esp)
 804bd83:	e8 38 27 00 00       	call   804e4c0 <__mcmp>
  if ((j > 0) || ((j == 0) && (dig & 1)))
 804bd88:	83 f8 00             	cmp    $0x0,%eax
 804bd8b:	8b 4c 24 24          	mov    0x24(%esp),%ecx
 804bd8f:	0f 8e 81 04 00 00    	jle    804c216 <_dtoa_r+0x101e>
 804bd95:	8a 57 ff             	mov    -0x1(%edi),%dl
 804bd98:	eb 07                	jmp    804bda1 <_dtoa_r+0xba9>
 804bd9a:	66 90                	xchg   %ax,%ax
 804bd9c:	8a 50 ff             	mov    -0x1(%eax),%dl
 804bd9f:	89 c7                	mov    %eax,%edi
    {
    roundoff:
      while (*--s == '9')
 804bda1:	8d 47 ff             	lea    -0x1(%edi),%eax
 804bda4:	80 fa 39             	cmp    $0x39,%dl
 804bda7:	0f 85 c5 04 00 00    	jne    804c272 <_dtoa_r+0x107a>
	if (s == s0)
 804bdad:	39 c6                	cmp    %eax,%esi
 804bdaf:	75 eb                	jne    804bd9c <_dtoa_r+0xba4>
	  {
	    k++;
 804bdb1:	ff 44 24 44          	incl   0x44(%esp)
	    *s++ = '1';
 804bdb5:	c6 06 31             	movb   $0x31,(%esi)
    {
      while (*--s == '0');
      s++;
    }
ret:
  Bfree (ptr, S);
 804bdb8:	8b 44 24 38          	mov    0x38(%esp),%eax
 804bdbc:	89 44 24 04          	mov    %eax,0x4(%esp)
 804bdc0:	89 2c 24             	mov    %ebp,(%esp)
 804bdc3:	89 4c 24 24          	mov    %ecx,0x24(%esp)
 804bdc7:	e8 70 20 00 00       	call   804de3c <_Bfree>
  if (mhi)
 804bdcc:	8b 44 24 4c          	mov    0x4c(%esp),%eax
 804bdd0:	85 c0                	test   %eax,%eax
 804bdd2:	8b 4c 24 24          	mov    0x24(%esp),%ecx
 804bdd6:	0f 84 ec f8 ff ff    	je     804b6c8 <_dtoa_r+0x4d0>
    {
      if (mlo && mlo != mhi)
 804bddc:	85 c9                	test   %ecx,%ecx
 804bdde:	74 12                	je     804bdf2 <_dtoa_r+0xbfa>
 804bde0:	3b 4c 24 4c          	cmp    0x4c(%esp),%ecx
 804bde4:	74 0c                	je     804bdf2 <_dtoa_r+0xbfa>
	Bfree (ptr, mlo);
 804bde6:	89 4c 24 04          	mov    %ecx,0x4(%esp)
 804bdea:	89 2c 24             	mov    %ebp,(%esp)
 804bded:	e8 4a 20 00 00       	call   804de3c <_Bfree>
      Bfree (ptr, mhi);
 804bdf2:	8b 44 24 4c          	mov    0x4c(%esp),%eax
 804bdf6:	89 44 24 04          	mov    %eax,0x4(%esp)
 804bdfa:	89 2c 24             	mov    %ebp,(%esp)
 804bdfd:	e8 3a 20 00 00       	call   804de3c <_Bfree>
 804be02:	e9 c1 f8 ff ff       	jmp    804b6c8 <_dtoa_r+0x4d0>
 804be07:	90                   	nop
      mode -= 4;
      try_quick = 0;
    }
  leftright = 1;
  ilim = ilim1 = -1;
  switch (mode)
 804be08:	83 bc 24 bc 00 00 00 	cmpl   $0x4,0xbc(%esp)
 804be0f:	04 
 804be10:	0f 84 27 03 00 00    	je     804c13d <_dtoa_r+0xf45>
 804be16:	83 bc 24 bc 00 00 00 	cmpl   $0x5,0xbc(%esp)
 804be1d:	05 
 804be1e:	0f 85 4f f9 ff ff    	jne    804b773 <_dtoa_r+0x57b>
  if (mode > 5)
    {
      mode -= 4;
      try_quick = 0;
    }
  leftright = 1;
 804be24:	c7 44 24 58 01 00 00 	movl   $0x1,0x58(%esp)
 804be2b:	00 
      break;
    case 3:
      leftright = 0;
      /* no break */
    case 5:
      i = ndigits + k + 1;
 804be2c:	8b 84 24 c0 00 00 00 	mov    0xc0(%esp),%eax
 804be33:	03 44 24 44          	add    0x44(%esp),%eax
 804be37:	89 44 24 68          	mov    %eax,0x68(%esp)
 804be3b:	89 c3                	mov    %eax,%ebx
 804be3d:	43                   	inc    %ebx
      ilim = i;
      ilim1 = i - 1;
      if (i <= 0)
 804be3e:	89 5c 24 40          	mov    %ebx,0x40(%esp)
 804be42:	85 db                	test   %ebx,%ebx
 804be44:	0f 8f 97 f6 ff ff    	jg     804b4e1 <_dtoa_r+0x2e9>
	i = 1;
    }
  j = sizeof (__ULong);
  for (_REENT_MP_RESULT_K(ptr) = 0; sizeof (_Bigint) - sizeof (__ULong) + j <= i;
 804be4a:	c7 45 44 00 00 00 00 	movl   $0x0,0x44(%ebp)
 804be51:	31 d2                	xor    %edx,%edx
 804be53:	e9 b5 f6 ff ff       	jmp    804b50d <_dtoa_r+0x315>
  /* Check for special case that d is a normalized power of 2. */

  spec_case = 0;
  if (mode < 2)
    {
      if (!word1 (d) && !(word0 (d) & Bndry_mask)
 804be58:	8b 44 24 30          	mov    0x30(%esp),%eax
 804be5c:	85 c0                	test   %eax,%eax
 804be5e:	0f 85 48 fc ff ff    	jne    804baac <_dtoa_r+0x8b4>
 804be64:	8b 44 24 34          	mov    0x34(%esp),%eax
 804be68:	a9 ff ff 0f 00       	test   $0xfffff,%eax
 804be6d:	0f 85 39 fc ff ff    	jne    804baac <_dtoa_r+0x8b4>
#ifndef Sudden_Underflow
	  && word0 (d) & Exp_mask
 804be73:	a9 00 00 f0 7f       	test   $0x7ff00000,%eax
 804be78:	0f 84 2e fc ff ff    	je     804baac <_dtoa_r+0x8b4>
#endif
	)
	{
	  /* The special case */
	  b2 += Log2P;
 804be7e:	ff 44 24 48          	incl   0x48(%esp)
	  s2 += Log2P;
 804be82:	ff 44 24 28          	incl   0x28(%esp)
	  spec_case = 1;
 804be86:	bf 01 00 00 00       	mov    $0x1,%edi
 804be8b:	e9 1e fc ff ff       	jmp    804baae <_dtoa_r+0x8b6>
      s2 += i;
      mhi = i2b (ptr, 1);
    }
  if (m2 > 0 && s2 > 0)
    {
      i = m2 < s2 ? m2 : s2;
 804be90:	89 d8                	mov    %ebx,%eax
 804be92:	e9 59 fb ff ff       	jmp    804b9f0 <_dtoa_r+0x7f8>

  if (be >= 0 && k <= Int_max)
    {
      /* Yes. */
      ds = tens[k];
      if (ndigits < 0 && ilim <= 0)
 804be97:	83 7c 24 40 00       	cmpl   $0x0,0x40(%esp)
 804be9c:	0f 8f 56 f9 ff ff    	jg     804b7f8 <_dtoa_r+0x600>
	{
	  S = mhi = 0;
	  if (ilim < 0 || d.d <= 5 * ds)
 804bea2:	0f 85 60 02 00 00    	jne    804c108 <_dtoa_r+0xf10>
 804bea8:	d8 0d 74 18 05 08    	fmuls  0x8051874
 804beae:	dd 44 24 30          	fldl   0x30(%esp)
 804beb2:	d9 c9                	fxch   %st(1)
 804beb4:	da e9                	fucompp 
 804beb6:	df e0                	fnstsw %ax
    {
      /* Yes. */
      ds = tens[k];
      if (ndigits < 0 && ilim <= 0)
	{
	  S = mhi = 0;
 804beb8:	c7 44 24 38 00 00 00 	movl   $0x0,0x38(%esp)
 804bebf:	00 
 804bec0:	c7 44 24 4c 00 00 00 	movl   $0x0,0x4c(%esp)
 804bec7:	00 
	  if (ilim < 0 || d.d <= 5 * ds)
 804bec8:	f6 c4 05             	test   $0x5,%ah
 804becb:	0f 85 e1 01 00 00    	jne    804c0b2 <_dtoa_r+0xeba>
    {
      if (ilim < 0 || cmp (b, S = multadd (ptr, S, 5, 0)) <= 0)
	{
	  /* no digits, fcvt style */
	no_digits:
	  k = -1 - ndigits;
 804bed1:	8b 94 24 c0 00 00 00 	mov    0xc0(%esp),%edx
 804bed8:	f7 d2                	not    %edx
 804beda:	89 54 24 44          	mov    %edx,0x44(%esp)
	  goto ret;
 804bede:	89 f7                	mov    %esi,%edi
    {
      while (*--s == '0');
      s++;
    }
ret:
  Bfree (ptr, S);
 804bee0:	8b 44 24 38          	mov    0x38(%esp),%eax
 804bee4:	89 44 24 04          	mov    %eax,0x4(%esp)
 804bee8:	89 2c 24             	mov    %ebp,(%esp)
 804beeb:	e8 4c 1f 00 00       	call   804de3c <_Bfree>
  if (mhi)
 804bef0:	83 7c 24 4c 00       	cmpl   $0x0,0x4c(%esp)
 804bef5:	0f 85 f7 fe ff ff    	jne    804bdf2 <_dtoa_r+0xbfa>
 804befb:	e9 c8 f7 ff ff       	jmp    804b6c8 <_dtoa_r+0x4d0>
	      b1 = mult (ptr, mhi, b);
	      Bfree (ptr, b);
	      b = b1;
	    }
         if ((j = b5 - m5) != 0)
	    b = pow5mult (ptr, b, j);
 804bf00:	89 44 24 08          	mov    %eax,0x8(%esp)
	}
      else
	b = pow5mult (ptr, b, b5);
 804bf04:	8b 44 24 54          	mov    0x54(%esp),%eax
 804bf08:	89 44 24 04          	mov    %eax,0x4(%esp)
 804bf0c:	89 2c 24             	mov    %ebp,(%esp)
 804bf0f:	e8 d8 23 00 00       	call   804e2ec <__pow5mult>
 804bf14:	89 44 24 54          	mov    %eax,0x54(%esp)
 804bf18:	e9 4d fb ff ff       	jmp    804ba6a <_dtoa_r+0x872>
    {
      if (mode < 2)
	{
	  i =
#ifndef Sudden_Underflow
	    denorm ? be + (Bias + (P - 1) - 1 + 1) :
 804bf1d:	8b 54 24 4c          	mov    0x4c(%esp),%edx
 804bf21:	85 d2                	test   %edx,%edx
 804bf23:	0f 84 c6 04 00 00    	je     804c3ef <_dtoa_r+0x11f7>
 804bf29:	05 33 04 00 00       	add    $0x433,%eax
 804bf2e:	8b 7c 24 38          	mov    0x38(%esp),%edi
	    break;
	}
      goto ret1;
    }

  m2 = b2;
 804bf32:	8b 5c 24 48          	mov    0x48(%esp),%ebx
 804bf36:	e9 0c fa ff ff       	jmp    804b947 <_dtoa_r+0x74f>
 804bf3b:	90                   	nop
	  /* Do we yet have the shortest decimal string
	   * that will round to d?
	   */
	  j = cmp (b, mlo);
	  delta = diff (ptr, S, mhi);
	  j1 = delta->_sign ? 1 : cmp (b, delta);
 804bf3c:	89 34 24             	mov    %esi,(%esp)
 804bf3f:	89 54 24 24          	mov    %edx,0x24(%esp)
 804bf43:	e8 78 25 00 00       	call   804e4c0 <__mcmp>
	  Bfree (ptr, delta);
 804bf48:	8b 54 24 24          	mov    0x24(%esp),%edx
 804bf4c:	89 54 24 04          	mov    %edx,0x4(%esp)
 804bf50:	89 2c 24             	mov    %ebp,(%esp)
 804bf53:	89 44 24 24          	mov    %eax,0x24(%esp)
 804bf57:	e8 e0 1e 00 00       	call   804de3c <_Bfree>
#ifndef ROUND_BIASED
	  if (j1 == 0 && !mode && !(word1 (d) & 1))
 804bf5c:	8b 44 24 24          	mov    0x24(%esp),%eax
 804bf60:	85 c0                	test   %eax,%eax
 804bf62:	0f 85 ab fc ff ff    	jne    804bc13 <_dtoa_r+0xa1b>
 804bf68:	8b 8c 24 bc 00 00 00 	mov    0xbc(%esp),%ecx
 804bf6f:	85 c9                	test   %ecx,%ecx
 804bf71:	0f 85 9c fc ff ff    	jne    804bc13 <_dtoa_r+0xa1b>
 804bf77:	8b 54 24 4c          	mov    0x4c(%esp),%edx
 804bf7b:	85 d2                	test   %edx,%edx
 804bf7d:	0f 85 90 fc ff ff    	jne    804bc13 <_dtoa_r+0xa1b>
 804bf83:	89 74 24 54          	mov    %esi,0x54(%esp)
 804bf87:	89 f9                	mov    %edi,%ecx
 804bf89:	8b 7c 24 3c          	mov    0x3c(%esp),%edi
 804bf8d:	8b 74 24 58          	mov    0x58(%esp),%esi
	    {
	      if (dig == '9')
 804bf91:	83 7c 24 30 39       	cmpl   $0x39,0x30(%esp)
 804bf96:	0f 84 61 02 00 00    	je     804c1fd <_dtoa_r+0x1005>
		goto round_9_up;
	      if (j > 0)
 804bf9c:	8b 5c 24 40          	mov    0x40(%esp),%ebx
 804bfa0:	85 db                	test   %ebx,%ebx
 804bfa2:	7e 07                	jle    804bfab <_dtoa_r+0xdb3>
		dig++;
 804bfa4:	8d 57 31             	lea    0x31(%edi),%edx
 804bfa7:	89 54 24 30          	mov    %edx,0x30(%esp)
		  j1 = cmp (b, S);
                 if (((j1 > 0) || ((j1 == 0) && (dig & 1)))
		      && dig++ == '9')
		    goto round_9_up;
		}
	      *s++ = dig;
 804bfab:	8a 44 24 30          	mov    0x30(%esp),%al
		{		/* possible if i == 1 */
		round_9_up:
		  *s++ = '9';
		  goto roundoff;
		}
	      *s++ = dig + 1;
 804bfaf:	8b 54 24 48          	mov    0x48(%esp),%edx
 804bfb3:	88 02                	mov    %al,(%edx)
 804bfb5:	89 d7                	mov    %edx,%edi
 804bfb7:	47                   	inc    %edi
	      goto ret;
 804bfb8:	8b 44 24 28          	mov    0x28(%esp),%eax
 804bfbc:	89 44 24 4c          	mov    %eax,0x4c(%esp)
 804bfc0:	e9 f3 fd ff ff       	jmp    804bdb8 <_dtoa_r+0xbc0>
 804bfc5:	8d 76 00             	lea    0x0(%esi),%esi
	  *s++ = dig;
	  if (i == ilim)
	    break;
	  b = multadd (ptr, b, 10, 0);
	  if (mlo == mhi)
	    mlo = mhi = multadd (ptr, mhi, 10, 0);
 804bfc8:	e8 8b 1e 00 00       	call   804de58 <__multadd>
 804bfcd:	89 c7                	mov    %eax,%edi
 804bfcf:	89 44 24 28          	mov    %eax,0x28(%esp)
 804bfd3:	43                   	inc    %ebx
 804bfd4:	e9 d7 fb ff ff       	jmp    804bbb0 <_dtoa_r+0x9b8>
 804bfd9:	8d 76 00             	lea    0x0(%esi),%esi
   * and for all and pass them and a shift to quorem, so it
   * can do shifts and ors to compute the numerator for q.
   */

#ifdef Pack_32
  if ((i = ((s5 ? 32 - hi0bits (S->_x[S->_wds - 1]) : 1) + s2) & 0x1f) != 0)
 804bfdc:	8b 54 24 38          	mov    0x38(%esp),%edx
 804bfe0:	8b 42 10             	mov    0x10(%edx),%eax
 804bfe3:	8b 44 82 10          	mov    0x10(%edx,%eax,4),%eax
 804bfe7:	89 04 24             	mov    %eax,(%esp)
 804bfea:	e8 41 20 00 00       	call   804e030 <__hi0bits>
 804bfef:	ba 20 00 00 00       	mov    $0x20,%edx
 804bff4:	29 c2                	sub    %eax,%edx
 804bff6:	e9 c4 fa ff ff       	jmp    804babf <_dtoa_r+0x8c7>
 804bffb:	90                   	nop
    b = lshift (ptr, b, b2);
  if (s2 > 0)
    S = lshift (ptr, S, s2);
  if (k_check)
    {
      if (cmp (b, S) < 0)
 804bffc:	8b 44 24 38          	mov    0x38(%esp),%eax
 804c000:	89 44 24 04          	mov    %eax,0x4(%esp)
 804c004:	8b 44 24 54          	mov    0x54(%esp),%eax
 804c008:	89 04 24             	mov    %eax,(%esp)
 804c00b:	e8 b0 24 00 00       	call   804e4c0 <__mcmp>
 804c010:	85 c0                	test   %eax,%eax
 804c012:	0f 89 2b fb ff ff    	jns    804bb43 <_dtoa_r+0x94b>
	{
	  k--;
 804c018:	ff 4c 24 44          	decl   0x44(%esp)
	  b = multadd (ptr, b, 10, 0);	/* we botched the k estimate */
 804c01c:	c7 44 24 0c 00 00 00 	movl   $0x0,0xc(%esp)
 804c023:	00 
 804c024:	c7 44 24 08 0a 00 00 	movl   $0xa,0x8(%esp)
 804c02b:	00 
 804c02c:	8b 44 24 54          	mov    0x54(%esp),%eax
 804c030:	89 44 24 04          	mov    %eax,0x4(%esp)
 804c034:	89 2c 24             	mov    %ebp,(%esp)
 804c037:	e8 1c 1e 00 00       	call   804de58 <__multadd>
 804c03c:	89 44 24 54          	mov    %eax,0x54(%esp)
	  if (leftright)
 804c040:	8b 44 24 58          	mov    0x58(%esp),%eax
 804c044:	85 c0                	test   %eax,%eax
 804c046:	0f 85 5d 04 00 00    	jne    804c4a9 <_dtoa_r+0x12b1>
 804c04c:	8b 54 24 68          	mov    0x68(%esp),%edx
 804c050:	89 54 24 40          	mov    %edx,0x40(%esp)
 804c054:	e9 ea fa ff ff       	jmp    804bb43 <_dtoa_r+0x94b>
 804c059:	8d 76 00             	lea    0x0(%esi),%esi
	    mhi = multadd (ptr, mhi, 10, 0);
	  ilim = ilim1;
	}
    }
  if (ilim <= 0 && mode > 2)
 804c05c:	83 bc 24 bc 00 00 00 	cmpl   $0x2,0xbc(%esp)
 804c063:	02 
 804c064:	0f 8e e5 fa ff ff    	jle    804bb4f <_dtoa_r+0x957>
    {
      if (ilim < 0 || cmp (b, S = multadd (ptr, S, 5, 0)) <= 0)
 804c06a:	8b 44 24 40          	mov    0x40(%esp),%eax
 804c06e:	85 c0                	test   %eax,%eax
 804c070:	0f 85 5b fe ff ff    	jne    804bed1 <_dtoa_r+0xcd9>
 804c076:	c7 44 24 0c 00 00 00 	movl   $0x0,0xc(%esp)
 804c07d:	00 
 804c07e:	c7 44 24 08 05 00 00 	movl   $0x5,0x8(%esp)
 804c085:	00 
 804c086:	8b 44 24 38          	mov    0x38(%esp),%eax
 804c08a:	89 44 24 04          	mov    %eax,0x4(%esp)
 804c08e:	89 2c 24             	mov    %ebp,(%esp)
 804c091:	e8 c2 1d 00 00       	call   804de58 <__multadd>
 804c096:	89 44 24 38          	mov    %eax,0x38(%esp)
 804c09a:	89 44 24 04          	mov    %eax,0x4(%esp)
 804c09e:	8b 44 24 54          	mov    0x54(%esp),%eax
 804c0a2:	89 04 24             	mov    %eax,(%esp)
 804c0a5:	e8 16 24 00 00       	call   804e4c0 <__mcmp>
 804c0aa:	85 c0                	test   %eax,%eax
 804c0ac:	0f 8e 1f fe ff ff    	jle    804bed1 <_dtoa_r+0xcd9>
	no_digits:
	  k = -1 - ndigits;
	  goto ret;
	}
    one_digit:
      *s++ = '1';
 804c0b2:	c6 06 31             	movb   $0x31,(%esi)
 804c0b5:	8d 7e 01             	lea    0x1(%esi),%edi
      k++;
 804c0b8:	ff 44 24 44          	incl   0x44(%esp)
      goto ret;
 804c0bc:	e9 1f fe ff ff       	jmp    804bee0 <_dtoa_r+0xce8>
	  ilim = ilim1;
	  k--;
	  d.d *= 10.;
	  ieps++;
	}
      eps.d = ieps * d.d + 7.;
 804c0c1:	d9 c0                	fld    %st(0)
 804c0c3:	52                   	push   %edx
 804c0c4:	da 0c 24             	fimull (%esp)
 804c0c7:	5f                   	pop    %edi
 804c0c8:	d8 05 70 18 05 08    	fadds  0x8051870
 804c0ce:	dd 5c 24 60          	fstpl  0x60(%esp)
      word0 (eps) -= (P - 1) * Exp_msk1;
 804c0d2:	81 6c 24 64 00 00 40 	subl   $0x3400000,0x64(%esp)
 804c0d9:	03 
 804c0da:	66 90                	xchg   %ax,%ax
      if (ilim == 0)
	{
	  S = mhi = 0;
	  d.d -= 5.;
 804c0dc:	d8 25 74 18 05 08    	fsubs  0x8051874
	  if (d.d > eps.d)
 804c0e2:	dd 44 24 60          	fldl   0x60(%esp)
 804c0e6:	d9 c9                	fxch   %st(1)
 804c0e8:	dd e1                	fucom  %st(1)
 804c0ea:	df e0                	fnstsw %ax
 804c0ec:	f6 c4 45             	test   $0x45,%ah
 804c0ef:	0f 84 15 02 00 00    	je     804c30a <_dtoa_r+0x1112>
 804c0f5:	d9 c9                	fxch   %st(1)
	    goto one_digit;
	  if (d.d < -eps.d)
 804c0f7:	d9 e0                	fchs   
 804c0f9:	da e9                	fucompp 
 804c0fb:	df e0                	fnstsw %ax
 804c0fd:	f6 c4 45             	test   $0x45,%ah
 804c100:	0f 85 be f6 ff ff    	jne    804b7c4 <_dtoa_r+0x5cc>
 804c106:	eb 02                	jmp    804c10a <_dtoa_r+0xf12>
 804c108:	dd d8                	fstp   %st(0)
    {
      /* Yes. */
      ds = tens[k];
      if (ndigits < 0 && ilim <= 0)
	{
	  S = mhi = 0;
 804c10a:	c7 44 24 38 00 00 00 	movl   $0x0,0x38(%esp)
 804c111:	00 
 804c112:	c7 44 24 4c 00 00 00 	movl   $0x0,0x4c(%esp)
 804c119:	00 
 804c11a:	e9 b2 fd ff ff       	jmp    804bed1 <_dtoa_r+0xcd9>
 804c11f:	90                   	nop
	    }
         if ((j = b5 - m5) != 0)
	    b = pow5mult (ptr, b, j);
	}
      else
	b = pow5mult (ptr, b, b5);
 804c120:	8b 54 24 38          	mov    0x38(%esp),%edx
 804c124:	89 54 24 08          	mov    %edx,0x8(%esp)
 804c128:	e9 d7 fd ff ff       	jmp    804bf04 <_dtoa_r+0xd0c>
 804c12d:	8d 76 00             	lea    0x0(%esi),%esi
      if (ndigits <= 0)
	ndigits = 1;
      ilim = ilim1 = i = ndigits;
      break;
    case 3:
      leftright = 0;
 804c130:	c7 44 24 58 00 00 00 	movl   $0x0,0x58(%esp)
 804c137:	00 
 804c138:	e9 ef fc ff ff       	jmp    804be2c <_dtoa_r+0xc34>
  if (mode > 5)
    {
      mode -= 4;
      try_quick = 0;
    }
  leftright = 1;
 804c13d:	c7 44 24 58 01 00 00 	movl   $0x1,0x58(%esp)
 804c144:	00 
 804c145:	e9 79 f3 ff ff       	jmp    804b4c3 <_dtoa_r+0x2cb>
	  j = ilim - 1;
	  if (m5 >= j)
	    m5 -= j;
	  else
	    {
	      s5 += j -= m5;
 804c14a:	89 c2                	mov    %eax,%edx
 804c14c:	2b 54 24 38          	sub    0x38(%esp),%edx
 804c150:	01 54 24 50          	add    %edx,0x50(%esp)
 804c154:	89 44 24 38          	mov    %eax,0x38(%esp)
	      b5 += j;
	      m5 = 0;
 804c158:	31 ff                	xor    %edi,%edi
 804c15a:	e9 d4 f7 ff ff       	jmp    804b933 <_dtoa_r+0x73b>
      ieps = 2;			/* conservative */
      if (k > 0)
	{
	  ds = tens[k & 0xf];
	  j = k >> 4;
	  if (j & Bletch)
 804c15f:	dd 44 24 30          	fldl   0x30(%esp)

      i = 0;
      d2.d = d.d;
      k0 = k;
      ilim0 = ilim;
      ieps = 2;			/* conservative */
 804c163:	ba 02 00 00 00       	mov    $0x2,%edx
 804c168:	e9 fd f3 ff ff       	jmp    804b56a <_dtoa_r+0x372>
      break;
    case 2:
      leftright = 0;
      /* no break */
    case 4:
      if (ndigits <= 0)
 804c16d:	bb 01 00 00 00       	mov    $0x1,%ebx
	ndigits = 1;
 804c172:	c7 44 24 68 01 00 00 	movl   $0x1,0x68(%esp)
 804c179:	00 
 804c17a:	c7 44 24 40 01 00 00 	movl   $0x1,0x40(%esp)
 804c181:	00 
 804c182:	c7 84 24 c0 00 00 00 	movl   $0x1,0xc0(%esp)
 804c189:	01 00 00 00 
 804c18d:	e9 b8 fc ff ff       	jmp    804be4a <_dtoa_r+0xc52>
 804c192:	89 74 24 54          	mov    %esi,0x54(%esp)
 804c196:	89 f9                	mov    %edi,%ecx
 804c198:	8b 7c 24 3c          	mov    0x3c(%esp),%edi
 804c19c:	8b 74 24 58          	mov    0x58(%esp),%esi
	  mhi = lshift (ptr, mhi, Log2P);
	}

      for (i = 1;; i++)
	{
	  dig = quorem (b, S) + '0';
 804c1a0:	8b 5c 24 30          	mov    0x30(%esp),%ebx
#ifndef ROUND_BIASED
	      && !(word1 (d) & 1)
#endif
           ))
	    {
	      if (j1 > 0)
 804c1a4:	85 c0                	test   %eax,%eax
 804c1a6:	0f 8e ff fd ff ff    	jle    804bfab <_dtoa_r+0xdb3>
		{
		  b = lshift (ptr, b, 1);
 804c1ac:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
 804c1b3:	00 
 804c1b4:	8b 44 24 54          	mov    0x54(%esp),%eax
 804c1b8:	89 44 24 04          	mov    %eax,0x4(%esp)
 804c1bc:	89 2c 24             	mov    %ebp,(%esp)
 804c1bf:	89 4c 24 24          	mov    %ecx,0x24(%esp)
 804c1c3:	e8 00 22 00 00       	call   804e3c8 <__lshift>
 804c1c8:	89 44 24 54          	mov    %eax,0x54(%esp)
		  j1 = cmp (b, S);
 804c1cc:	8b 44 24 38          	mov    0x38(%esp),%eax
 804c1d0:	89 44 24 04          	mov    %eax,0x4(%esp)
 804c1d4:	8b 44 24 54          	mov    0x54(%esp),%eax
 804c1d8:	89 04 24             	mov    %eax,(%esp)
 804c1db:	e8 e0 22 00 00       	call   804e4c0 <__mcmp>
                 if (((j1 > 0) || ((j1 == 0) && (dig & 1)))
 804c1e0:	83 f8 00             	cmp    $0x0,%eax
 804c1e3:	8b 4c 24 24          	mov    0x24(%esp),%ecx
 804c1e7:	0f 8e a6 02 00 00    	jle    804c493 <_dtoa_r+0x129b>
		      && dig++ == '9')
 804c1ed:	8d 57 31             	lea    0x31(%edi),%edx
 804c1f0:	89 54 24 30          	mov    %edx,0x30(%esp)
 804c1f4:	83 fb 39             	cmp    $0x39,%ebx
 804c1f7:	0f 85 ae fd ff ff    	jne    804bfab <_dtoa_r+0xdb3>
	  if (j1 > 0)
	    {
	      if (dig == '9')
		{		/* possible if i == 1 */
		round_9_up:
		  *s++ = '9';
 804c1fd:	8b 54 24 48          	mov    0x48(%esp),%edx
 804c201:	c6 02 39             	movb   $0x39,(%edx)
 804c204:	89 d7                	mov    %edx,%edi
 804c206:	47                   	inc    %edi
		  goto roundoff;
 804c207:	8b 44 24 28          	mov    0x28(%esp),%eax
 804c20b:	89 44 24 4c          	mov    %eax,0x4c(%esp)
 804c20f:	b2 39                	mov    $0x39,%dl
 804c211:	e9 8b fb ff ff       	jmp    804bda1 <_dtoa_r+0xba9>

  /* Round off last digit */

  b = lshift (ptr, b, 1);
  j = cmp (b, S);
  if ((j > 0) || ((j == 0) && (dig & 1)))
 804c216:	75 0e                	jne    804c226 <_dtoa_r+0x102e>
 804c218:	f6 44 24 30 01       	testb  $0x1,0x30(%esp)
 804c21d:	74 07                	je     804c226 <_dtoa_r+0x102e>
 804c21f:	e9 71 fb ff ff       	jmp    804bd95 <_dtoa_r+0xb9d>
	  }
      ++*s++;
    }
  else
    {
      while (*--s == '0');
 804c224:	89 c7                	mov    %eax,%edi
 804c226:	8d 47 ff             	lea    -0x1(%edi),%eax
 804c229:	80 7f ff 30          	cmpb   $0x30,-0x1(%edi)
 804c22d:	74 f5                	je     804c224 <_dtoa_r+0x102c>
 804c22f:	e9 84 fb ff ff       	jmp    804bdb8 <_dtoa_r+0xbc0>
		ieps++;
		ds *= bigtens[i];
	      }
	  d.d /= ds;
	}
      else if ((j1 = -k) != 0)
 804c234:	f7 d8                	neg    %eax
 804c236:	0f 84 cc 01 00 00    	je     804c408 <_dtoa_r+0x1210>
	{
	  d.d *= tens[j1 & 0xf];
 804c23c:	89 c2                	mov    %eax,%edx
 804c23e:	83 e2 0f             	and    $0xf,%edx
 804c241:	dd 44 24 30          	fldl   0x30(%esp)
 804c245:	dc 0c d5 20 19 05 08 	fmull  0x8051920(,%edx,8)
	  for (j = j1 >> 4; j; j >>= 1, i++)
 804c24c:	ba 02 00 00 00       	mov    $0x2,%edx
 804c251:	c1 f8 04             	sar    $0x4,%eax
 804c254:	0f 84 2d f3 ff ff    	je     804b587 <_dtoa_r+0x38f>
 804c25a:	31 c9                	xor    %ecx,%ecx
	    if (j & 1)
 804c25c:	a8 01                	test   $0x1,%al
 804c25e:	74 08                	je     804c268 <_dtoa_r+0x1070>
	      {
		ieps++;
 804c260:	42                   	inc    %edx
		d.d *= bigtens[i];
 804c261:	dc 0c cd e0 18 05 08 	fmull  0x80518e0(,%ecx,8)
	  d.d /= ds;
	}
      else if ((j1 = -k) != 0)
	{
	  d.d *= tens[j1 & 0xf];
	  for (j = j1 >> 4; j; j >>= 1, i++)
 804c268:	41                   	inc    %ecx
 804c269:	d1 f8                	sar    %eax
 804c26b:	75 ef                	jne    804c25c <_dtoa_r+0x1064>
 804c26d:	e9 15 f3 ff ff       	jmp    804b587 <_dtoa_r+0x38f>
	  {
	    k++;
	    *s++ = '1';
	    goto ret;
	  }
      ++*s++;
 804c272:	42                   	inc    %edx
 804c273:	88 10                	mov    %dl,(%eax)
 804c275:	e9 3e fb ff ff       	jmp    804bdb8 <_dtoa_r+0xbc0>
       */

      mlo = mhi;
      if (spec_case)
	{
	  mhi = Balloc (ptr, mhi->_k);
 804c27a:	8b 54 24 4c          	mov    0x4c(%esp),%edx
 804c27e:	8b 42 04             	mov    0x4(%edx),%eax
 804c281:	89 44 24 04          	mov    %eax,0x4(%esp)
 804c285:	89 2c 24             	mov    %ebp,(%esp)
 804c288:	e8 23 1b 00 00       	call   804ddb0 <_Balloc>
 804c28d:	89 c3                	mov    %eax,%ebx
	  Bcopy (mhi, mlo);
 804c28f:	8b 54 24 4c          	mov    0x4c(%esp),%edx
 804c293:	8b 42 10             	mov    0x10(%edx),%eax
 804c296:	8d 04 85 08 00 00 00 	lea    0x8(,%eax,4),%eax
 804c29d:	89 44 24 08          	mov    %eax,0x8(%esp)
 804c2a1:	89 d0                	mov    %edx,%eax
 804c2a3:	83 c0 0c             	add    $0xc,%eax
 804c2a6:	89 44 24 04          	mov    %eax,0x4(%esp)
 804c2aa:	8d 43 0c             	lea    0xc(%ebx),%eax
 804c2ad:	89 04 24             	mov    %eax,(%esp)
 804c2b0:	e8 93 19 00 00       	call   804dc48 <memcpy>
	  mhi = lshift (ptr, mhi, Log2P);
 804c2b5:	c7 44 24 08 01 00 00 	movl   $0x1,0x8(%esp)
 804c2bc:	00 
 804c2bd:	89 5c 24 04          	mov    %ebx,0x4(%esp)
 804c2c1:	89 2c 24             	mov    %ebp,(%esp)
 804c2c4:	e8 ff 20 00 00       	call   804e3c8 <__lshift>
 804c2c9:	89 44 24 28          	mov    %eax,0x28(%esp)
 804c2cd:	e9 b5 f8 ff ff       	jmp    804bb87 <_dtoa_r+0x98f>
 804c2d2:	89 74 24 54          	mov    %esi,0x54(%esp)
 804c2d6:	89 f9                	mov    %edi,%ecx
 804c2d8:	8b 74 24 58          	mov    0x58(%esp),%esi
	      *s++ = dig;
	      goto ret;
	    }
	  if (j1 > 0)
	    {
	      if (dig == '9')
 804c2dc:	83 7c 24 30 39       	cmpl   $0x39,0x30(%esp)
 804c2e1:	0f 84 16 ff ff ff    	je     804c1fd <_dtoa_r+0x1005>
		{		/* possible if i == 1 */
		round_9_up:
		  *s++ = '9';
		  goto roundoff;
		}
	      *s++ = dig + 1;
 804c2e7:	8a 44 24 30          	mov    0x30(%esp),%al
 804c2eb:	40                   	inc    %eax
 804c2ec:	e9 be fc ff ff       	jmp    804bfaf <_dtoa_r+0xdb7>
 804c2f1:	89 74 24 54          	mov    %esi,0x54(%esp)
 804c2f5:	89 f9                	mov    %edi,%ecx
 804c2f7:	89 df                	mov    %ebx,%edi
 804c2f9:	8b 74 24 58          	mov    0x58(%esp),%esi
	      goto ret;
	    }
	  *s++ = dig;
	  if (i == ilim)
 804c2fd:	8b 44 24 28          	mov    0x28(%esp),%eax
 804c301:	89 44 24 4c          	mov    %eax,0x4c(%esp)
 804c305:	e9 4a fa ff ff       	jmp    804bd54 <_dtoa_r+0xb5c>
 804c30a:	dd d8                	fstp   %st(0)
 804c30c:	dd d8                	fstp   %st(0)
	}
      eps.d = ieps * d.d + 7.;
      word0 (eps) -= (P - 1) * Exp_msk1;
      if (ilim == 0)
	{
	  S = mhi = 0;
 804c30e:	c7 44 24 38 00 00 00 	movl   $0x0,0x38(%esp)
 804c315:	00 
 804c316:	c7 44 24 4c 00 00 00 	movl   $0x0,0x4c(%esp)
 804c31d:	00 
 804c31e:	e9 8f fd ff ff       	jmp    804c0b2 <_dtoa_r+0xeba>
	}
      else
	{
#endif
	  /* Generate ilim digits, then fix them up. */
	  eps.d *= tens[ilim - 1];
 804c323:	8d 41 ff             	lea    -0x1(%ecx),%eax
 804c326:	89 44 24 6c          	mov    %eax,0x6c(%esp)
 804c32a:	dd 44 24 60          	fldl   0x60(%esp)
 804c32e:	dc 0c c5 20 19 05 08 	fmull  0x8051920(,%eax,8)
 804c335:	d9 c9                	fxch   %st(1)
	  for (i = 1;; i++, d.d *= 10.)
	    {
	      L = d.d;
 804c337:	d9 7c 24 7e          	fnstcw 0x7e(%esp)
 804c33b:	66 8b 44 24 7e       	mov    0x7e(%esp),%ax
 804c340:	b4 0c                	mov    $0xc,%ah
 804c342:	66 89 44 24 7c       	mov    %ax,0x7c(%esp)
 804c347:	d9 6c 24 7c          	fldcw  0x7c(%esp)
 804c34b:	db 54 24 78          	fistl  0x78(%esp)
 804c34f:	d9 6c 24 7e          	fldcw  0x7e(%esp)
 804c353:	8b 44 24 78          	mov    0x78(%esp),%eax
	      d.d -= L;
 804c357:	50                   	push   %eax
 804c358:	da 24 24             	fisubl (%esp)
 804c35b:	5a                   	pop    %edx
	      *s++ = '0' + (int) L;
 804c35c:	83 c0 30             	add    $0x30,%eax
 804c35f:	88 06                	mov    %al,(%esi)
 804c361:	8d 7e 01             	lea    0x1(%esi),%edi
	      if (i == ilim)
 804c364:	83 f9 01             	cmp    $0x1,%ecx
 804c367:	74 3c                	je     804c3a5 <_dtoa_r+0x11ad>
 804c369:	01 f1                	add    %esi,%ecx
	  eps.d *= tens[ilim - 1];
	  for (i = 1;; i++, d.d *= 10.)
	    {
	      L = d.d;
	      d.d -= L;
	      *s++ = '0' + (int) L;
 804c36b:	89 f8                	mov    %edi,%eax
      else
	{
#endif
	  /* Generate ilim digits, then fix them up. */
	  eps.d *= tens[ilim - 1];
	  for (i = 1;; i++, d.d *= 10.)
 804c36d:	d9 05 6c 18 05 08    	flds   0x805186c
 804c373:	eb 05                	jmp    804c37a <_dtoa_r+0x1182>
 804c375:	8d 76 00             	lea    0x0(%esi),%esi
 804c378:	d9 c9                	fxch   %st(1)
 804c37a:	dc c9                	fmul   %st,%st(1)
 804c37c:	d9 c9                	fxch   %st(1)
	    {
	      L = d.d;
 804c37e:	d9 6c 24 7c          	fldcw  0x7c(%esp)
 804c382:	db 54 24 78          	fistl  0x78(%esp)
 804c386:	d9 6c 24 7e          	fldcw  0x7e(%esp)
 804c38a:	8b 54 24 78          	mov    0x78(%esp),%edx
	      d.d -= L;
 804c38e:	52                   	push   %edx
 804c38f:	da 24 24             	fisubl (%esp)
 804c392:	83 c4 04             	add    $0x4,%esp
	      *s++ = '0' + (int) L;
 804c395:	83 c2 30             	add    $0x30,%edx
 804c398:	88 10                	mov    %dl,(%eax)
 804c39a:	40                   	inc    %eax
	      if (i == ilim)
 804c39b:	39 c8                	cmp    %ecx,%eax
 804c39d:	75 d9                	jne    804c378 <_dtoa_r+0x1180>
 804c39f:	dd d9                	fstp   %st(1)
 804c3a1:	03 7c 24 6c          	add    0x6c(%esp),%edi
		{
		  if (d.d > 0.5 + eps.d)
 804c3a5:	d9 05 78 18 05 08    	flds   0x8051878
 804c3ab:	d9 c2                	fld    %st(2)
 804c3ad:	d8 c1                	fadd   %st(1),%st
 804c3af:	d9 ca                	fxch   %st(2)
 804c3b1:	dd e2                	fucom  %st(2)
 804c3b3:	df e0                	fnstsw %ax
 804c3b5:	dd da                	fstp   %st(2)
 804c3b7:	f6 c4 45             	test   $0x45,%ah
 804c3ba:	0f 85 9c 00 00 00    	jne    804c45c <_dtoa_r+0x1264>
 804c3c0:	dd d8                	fstp   %st(0)
 804c3c2:	dd d8                	fstp   %st(0)
 804c3c4:	dd d8                	fstp   %st(0)
 804c3c6:	8a 57 ff             	mov    -0x1(%edi),%dl
 804c3c9:	89 5c 24 44          	mov    %ebx,0x44(%esp)
 804c3cd:	e9 0f f5 ff ff       	jmp    804b8e1 <_dtoa_r+0x6e9>
	      b5 += j;
	      m5 = 0;
	    }
	  if ((i = ilim) < 0)
	    {
	      m2 -= i;
 804c3d2:	2b 5c 24 40          	sub    0x40(%esp),%ebx
	      i = 0;
 804c3d6:	31 c0                	xor    %eax,%eax
 804c3d8:	e9 6a f5 ff ff       	jmp    804b947 <_dtoa_r+0x74f>
 804c3dd:	bb 01 00 00 00       	mov    $0x1,%ebx
 804c3e2:	e9 68 f9 ff ff       	jmp    804bd4f <_dtoa_r+0xb57>
 804c3e7:	42                   	inc    %edx
		      {
			k++;
			*s = '0';
			break;
		      }
		  ++*s++;
 804c3e8:	88 10                	mov    %dl,(%eax)
 804c3ea:	e9 d9 f2 ff ff       	jmp    804b6c8 <_dtoa_r+0x4d0>
    {
      if (mode < 2)
	{
	  i =
#ifndef Sudden_Underflow
	    denorm ? be + (Bias + (P - 1) - 1 + 1) :
 804c3ef:	b8 36 00 00 00       	mov    $0x36,%eax
 804c3f4:	2b 84 24 88 00 00 00 	sub    0x88(%esp),%eax
 804c3fb:	8b 7c 24 38          	mov    0x38(%esp),%edi
	    break;
	}
      goto ret1;
    }

  m2 = b2;
 804c3ff:	8b 5c 24 48          	mov    0x48(%esp),%ebx
 804c403:	e9 3f f5 ff ff       	jmp    804b947 <_dtoa_r+0x74f>
		ieps++;
		ds *= bigtens[i];
	      }
	  d.d /= ds;
	}
      else if ((j1 = -k) != 0)
 804c408:	dd 44 24 30          	fldl   0x30(%esp)

      i = 0;
      d2.d = d.d;
      k0 = k;
      ilim0 = ilim;
      ieps = 2;			/* conservative */
 804c40c:	ba 02 00 00 00       	mov    $0x2,%edx
 804c411:	e9 71 f1 ff ff       	jmp    804b587 <_dtoa_r+0x38f>
	      {
		ieps++;
		d.d *= bigtens[i];
	      }
	}
      if (k_check && d.d < 1. && ilim > 0)
 804c416:	8b 5c 24 40          	mov    0x40(%esp),%ebx
 804c41a:	85 db                	test   %ebx,%ebx
 804c41c:	0f 84 9f fc ff ff    	je     804c0c1 <_dtoa_r+0xec9>
	{
	  if (ilim1 <= 0)
 804c422:	8b 4c 24 68          	mov    0x68(%esp),%ecx
 804c426:	85 c9                	test   %ecx,%ecx
 804c428:	0f 8e 94 f3 ff ff    	jle    804b7c2 <_dtoa_r+0x5ca>
	    goto fast_failed;
	  ilim = ilim1;
	  k--;
 804c42e:	8b 5c 24 44          	mov    0x44(%esp),%ebx
 804c432:	4b                   	dec    %ebx
	  d.d *= 10.;
 804c433:	d8 0d 6c 18 05 08    	fmuls  0x805186c
	  ieps++;
 804c439:	42                   	inc    %edx
	}
      eps.d = ieps * d.d + 7.;
 804c43a:	d9 c0                	fld    %st(0)
 804c43c:	52                   	push   %edx
 804c43d:	da 0c 24             	fimull (%esp)
 804c440:	58                   	pop    %eax
 804c441:	d8 05 70 18 05 08    	fadds  0x8051870
 804c447:	dd 5c 24 60          	fstpl  0x60(%esp)
      word0 (eps) -= (P - 1) * Exp_msk1;
 804c44b:	81 6c 24 64 00 00 40 	subl   $0x3400000,0x64(%esp)
 804c452:	03 
 804c453:	8b 4c 24 68          	mov    0x68(%esp),%ecx
 804c457:	e9 6f f1 ff ff       	jmp    804b5cb <_dtoa_r+0x3d3>
	      *s++ = '0' + (int) L;
	      if (i == ilim)
		{
		  if (d.d > 0.5 + eps.d)
		    goto bump_up;
		  else if (d.d < 0.5 - eps.d)
 804c45c:	de e2                	fsubp  %st,%st(2)
 804c45e:	d9 c9                	fxch   %st(1)
 804c460:	da e9                	fucompp 
 804c462:	df e0                	fnstsw %ax
 804c464:	f6 c4 45             	test   $0x45,%ah
 804c467:	74 07                	je     804c470 <_dtoa_r+0x1278>
 804c469:	e9 56 f3 ff ff       	jmp    804b7c4 <_dtoa_r+0x5cc>
		    {
		      while (*--s == '0');
 804c46e:	89 c7                	mov    %eax,%edi
 804c470:	8d 47 ff             	lea    -0x1(%edi),%eax
 804c473:	80 7f ff 30          	cmpb   $0x30,-0x1(%edi)
 804c477:	74 f5                	je     804c46e <_dtoa_r+0x1276>
 804c479:	e9 33 f2 ff ff       	jmp    804b6b1 <_dtoa_r+0x4b9>
 804c47e:	dd d8                	fstp   %st(0)
 804c480:	dd d8                	fstp   %st(0)
 804c482:	eb 06                	jmp    804c48a <_dtoa_r+0x1292>
 804c484:	dd d8                	fstp   %st(0)
 804c486:	dd d8                	fstp   %st(0)
 804c488:	dd d8                	fstp   %st(0)
	      L = d.d;
	      d.d -= L;
	      *s++ = '0' + (int) L;
	      if (d.d < eps.d)
		goto ret1;
	      if (1. - d.d < eps.d)
 804c48a:	89 5c 24 44          	mov    %ebx,0x44(%esp)
 804c48e:	e9 4e f4 ff ff       	jmp    804b8e1 <_dtoa_r+0x6e9>
	    {
	      if (j1 > 0)
		{
		  b = lshift (ptr, b, 1);
		  j1 = cmp (b, S);
                 if (((j1 > 0) || ((j1 == 0) && (dig & 1)))
 804c493:	0f 85 12 fb ff ff    	jne    804bfab <_dtoa_r+0xdb3>
 804c499:	f6 44 24 30 01       	testb  $0x1,0x30(%esp)
 804c49e:	0f 84 07 fb ff ff    	je     804bfab <_dtoa_r+0xdb3>
 804c4a4:	e9 44 fd ff ff       	jmp    804c1ed <_dtoa_r+0xff5>
      if (cmp (b, S) < 0)
	{
	  k--;
	  b = multadd (ptr, b, 10, 0);	/* we botched the k estimate */
	  if (leftright)
	    mhi = multadd (ptr, mhi, 10, 0);
 804c4a9:	c7 44 24 0c 00 00 00 	movl   $0x0,0xc(%esp)
 804c4b0:	00 
 804c4b1:	c7 44 24 08 0a 00 00 	movl   $0xa,0x8(%esp)
 804c4b8:	00 
 804c4b9:	8b 44 24 4c          	mov    0x4c(%esp),%eax
 804c4bd:	89 44 24 04          	mov    %eax,0x4(%esp)
 804c4c1:	89 2c 24             	mov    %ebp,(%esp)
 804c4c4:	e8 8f 19 00 00       	call   804de58 <__multadd>
 804c4c9:	89 44 24 4c          	mov    %eax,0x4c(%esp)
 804c4cd:	8b 54 24 68          	mov    0x68(%esp),%edx
 804c4d1:	89 54 24 40          	mov    %edx,0x40(%esp)
 804c4d5:	e9 69 f6 ff ff       	jmp    804bb43 <_dtoa_r+0x94b>
      i -= 4;
      b2 += i;
      m2 += i;
      s2 += i;
    }
  else if (i < 4)
 804c4da:	0f 84 0f f6 ff ff    	je     804baef <_dtoa_r+0x8f7>
 804c4e0:	ba 3c 00 00 00       	mov    $0x3c,%edx
 804c4e5:	29 c2                	sub    %eax,%edx
 804c4e7:	e9 d9 f7 ff ff       	jmp    804bcc5 <_dtoa_r+0xacd>
      b5 = -k;
      s5 = 0;
    }
  if (mode < 0 || mode > 9)
    mode = 0;
  try_quick = 1;
 804c4ec:	bf 01 00 00 00       	mov    $0x1,%edi
 804c4f1:	e9 a3 ef ff ff       	jmp    804b499 <_dtoa_r+0x2a1>
      ilim1 = i - 1;
      if (i <= 0)
	i = 1;
    }
  j = sizeof (__ULong);
  for (_REENT_MP_RESULT_K(ptr) = 0; sizeof (_Bigint) - sizeof (__ULong) + j <= i;
 804c4f6:	31 d2                	xor    %edx,%edx
 804c4f8:	e9 10 f0 ff ff       	jmp    804b50d <_dtoa_r+0x315>
 804c4fd:	90                   	nop
 804c4fe:	90                   	nop
 804c4ff:	90                   	nop

0804c500 <__errno>:

int *
__errno ()
{
  return &_REENT->_errno;
}
 804c500:	a1 40 40 05 08       	mov    0x8054040,%eax
 804c505:	c3                   	ret    
 804c506:	90                   	nop
 804c507:	90                   	nop

0804c508 <__sflush_r>:
   directly from __srefill. */
int
_DEFUN(__sflush_r, (ptr, fp),
       struct _reent *ptr _AND
       register FILE * fp)
{
 804c508:	55                   	push   %ebp
 804c509:	57                   	push   %edi
 804c50a:	56                   	push   %esi
 804c50b:	53                   	push   %ebx
 804c50c:	83 ec 1c             	sub    $0x1c,%esp
 804c50f:	8b 7c 24 30          	mov    0x30(%esp),%edi
 804c513:	8b 74 24 34          	mov    0x34(%esp),%esi
  register unsigned char *p;
  register _READ_WRITE_BUFSIZE_TYPE n;
  register _READ_WRITE_RETURN_TYPE t;
  short flags;

  flags = fp->_flags;
 804c517:	8b 46 0c             	mov    0xc(%esi),%eax
  if ((flags & __SWR) == 0)
 804c51a:	a8 08                	test   $0x8,%al
 804c51c:	0f 85 b6 00 00 00    	jne    804c5d8 <__sflush_r+0xd0>
    {
#ifdef _FSEEK_OPTIMIZATION
      /* For a read stream, an fflush causes the next seek to be
         unoptimized (i.e. forces a system-level seek).  This conforms
         to the POSIX and SUSv3 standards.  */
      fp->_flags |= __SNPT;
 804c522:	89 c1                	mov    %eax,%ecx
 804c524:	80 cd 08             	or     $0x8,%ch
 804c527:	66 89 4e 0c          	mov    %cx,0xc(%esi)
         the next byte from the file rather than the buffer.  This conforms
         to the POSIX and SUSv3 standards.  Note that the standards allow
         this seek to be deferred until necessary, but we choose to do it here
         to make the change simpler, more contained, and less likely
         to miss a code scenario.  */
      if ((fp->_r > 0 || fp->_ur > 0) && fp->_seek != NULL)
 804c52b:	8b 6e 04             	mov    0x4(%esi),%ebp
 804c52e:	85 ed                	test   %ebp,%ebp
 804c530:	0f 8e fa 00 00 00    	jle    804c630 <__sflush_r+0x128>
 804c536:	8b 56 28             	mov    0x28(%esi),%edx
 804c539:	85 d2                	test   %edx,%edx
 804c53b:	0f 84 8b 00 00 00    	je     804c5cc <__sflush_r+0xc4>
#endif

	  /* Save last errno and set errno to 0, so we can check if a device
	     returns with a valid position -1.  We restore the last errno if
	     no other error condition has been encountered. */
	  tmp_errno = ptr->_errno;
 804c541:	8b 1f                	mov    (%edi),%ebx
	  ptr->_errno = 0;
 804c543:	c7 07 00 00 00 00    	movl   $0x0,(%edi)
	  /* Get the physical position we are at in the file.  */
	  if (fp->_flags & __SOFF)
 804c549:	f6 c4 10             	test   $0x10,%ah
 804c54c:	0f 84 ee 00 00 00    	je     804c640 <__sflush_r+0x138>
	    curoff = fp->_offset;
 804c552:	8b 46 50             	mov    0x50(%esi),%eax
		  else
		    fp->_flags |= __SERR;
		  return result;
		}
            }
          if (fp->_flags & __SRD)
 804c555:	83 e1 04             	and    $0x4,%ecx
 804c558:	74 0d                	je     804c567 <__sflush_r+0x5f>
            {
              /* Current offset is at end of buffer.  Compensate for
                 characters not yet read.  */
              curoff -= fp->_r;
 804c55a:	2b 46 04             	sub    0x4(%esi),%eax
              if (HASUB (fp))
 804c55d:	8b 4e 30             	mov    0x30(%esi),%ecx
 804c560:	85 c9                	test   %ecx,%ecx
 804c562:	74 03                	je     804c567 <__sflush_r+0x5f>
                curoff -= fp->_ur;
 804c564:	2b 46 3c             	sub    0x3c(%esi),%eax
#ifdef __LARGE64_FILES
	  if (fp->_flags & __SL64)
	    curoff = fp->_seek64 (ptr, fp->_cookie, curoff, SEEK_SET);
	  else
#endif
	    curoff = fp->_seek (ptr, fp->_cookie, curoff, SEEK_SET);
 804c567:	c7 44 24 0c 00 00 00 	movl   $0x0,0xc(%esp)
 804c56e:	00 
 804c56f:	89 44 24 08          	mov    %eax,0x8(%esp)
 804c573:	8b 46 1c             	mov    0x1c(%esi),%eax
 804c576:	89 44 24 04          	mov    %eax,0x4(%esp)
 804c57a:	89 3c 24             	mov    %edi,(%esp)
 804c57d:	ff d2                	call   *%edx
	  if (curoff != -1 || ptr->_errno == 0
 804c57f:	83 f8 ff             	cmp    $0xffffffff,%eax
 804c582:	0f 84 e4 00 00 00    	je     804c66c <__sflush_r+0x164>
	      || ptr->_errno == ESPIPE || ptr->_errno == EINVAL)
	    {
	      /* Seek successful or ignorable error condition.
		 We can clear read buffer now.  */
#ifdef _FSEEK_OPTIMIZATION
	      fp->_flags &= ~__SNPT;
 804c588:	8b 56 0c             	mov    0xc(%esi),%edx
 804c58b:	89 d1                	mov    %edx,%ecx
 804c58d:	80 e5 f7             	and    $0xf7,%ch
 804c590:	66 89 4e 0c          	mov    %cx,0xc(%esi)
#endif
	      fp->_r = 0;
 804c594:	c7 46 04 00 00 00 00 	movl   $0x0,0x4(%esi)
	      fp->_p = fp->_bf._base;
 804c59b:	8b 4e 10             	mov    0x10(%esi),%ecx
 804c59e:	89 0e                	mov    %ecx,(%esi)
	      if ((fp->_flags & __SOFF) && (curoff != -1 || ptr->_errno == 0))
 804c5a0:	80 e6 10             	and    $0x10,%dh
 804c5a3:	0f 85 0c 01 00 00    	jne    804c6b5 <__sflush_r+0x1ad>
		fp->_offset = curoff;
	      ptr->_errno = tmp_errno;
 804c5a9:	89 1f                	mov    %ebx,(%edi)
	      if (HASUB (fp))
 804c5ab:	8b 46 30             	mov    0x30(%esi),%eax
 804c5ae:	85 c0                	test   %eax,%eax
 804c5b0:	74 1a                	je     804c5cc <__sflush_r+0xc4>
		FREEUB (ptr, fp);
 804c5b2:	8d 56 40             	lea    0x40(%esi),%edx
 804c5b5:	39 d0                	cmp    %edx,%eax
 804c5b7:	74 0c                	je     804c5c5 <__sflush_r+0xbd>
 804c5b9:	89 44 24 04          	mov    %eax,0x4(%esp)
 804c5bd:	89 3c 24             	mov    %edi,(%esp)
 804c5c0:	e8 3b 05 00 00       	call   804cb00 <_free_r>
 804c5c5:	c7 46 30 00 00 00 00 	movl   $0x0,0x30(%esi)
	    {
	      fp->_flags |= __SERR;
	      return EOF;
	    }
	}
      return 0;
 804c5cc:	31 c0                	xor    %eax,%eax
	}
      p += t;
      n -= t;
    }
  return 0;
}
 804c5ce:	83 c4 1c             	add    $0x1c,%esp
 804c5d1:	5b                   	pop    %ebx
 804c5d2:	5e                   	pop    %esi
 804c5d3:	5f                   	pop    %edi
 804c5d4:	5d                   	pop    %ebp
 804c5d5:	c3                   	ret    
 804c5d6:	66 90                	xchg   %ax,%ax
	      return EOF;
	    }
	}
      return 0;
    }
  if ((p = fp->_bf._base) == NULL)
 804c5d8:	8b 6e 10             	mov    0x10(%esi),%ebp
 804c5db:	85 ed                	test   %ebp,%ebp
 804c5dd:	74 ed                	je     804c5cc <__sflush_r+0xc4>
    {
      /* Nothing to flush.  */
      return 0;
    }
  n = fp->_p - p;		/* write this much */
 804c5df:	8b 1e                	mov    (%esi),%ebx
 804c5e1:	29 eb                	sub    %ebp,%ebx
  /*
   * Set these immediately to avoid problems with longjmp
   * and to allow exchange buffering (via setvbuf) in user
   * write function.
   */
  fp->_p = p;
 804c5e3:	89 2e                	mov    %ebp,(%esi)
  fp->_w = flags & (__SLBF | __SNBF) ? 0 : fp->_bf._size;
 804c5e5:	a8 03                	test   $0x3,%al
 804c5e7:	75 43                	jne    804c62c <__sflush_r+0x124>
 804c5e9:	8b 46 14             	mov    0x14(%esi),%eax
 804c5ec:	89 46 08             	mov    %eax,0x8(%esi)

  while (n > 0)
 804c5ef:	85 db                	test   %ebx,%ebx
 804c5f1:	7f 0d                	jg     804c600 <__sflush_r+0xf8>
 804c5f3:	eb d7                	jmp    804c5cc <__sflush_r+0xc4>
 804c5f5:	8d 76 00             	lea    0x0(%esi),%esi
      if (t <= 0)
	{
          fp->_flags |= __SERR;
          return EOF;
	}
      p += t;
 804c5f8:	01 c5                	add    %eax,%ebp
      n -= t;
 804c5fa:	29 c3                	sub    %eax,%ebx
   * write function.
   */
  fp->_p = p;
  fp->_w = flags & (__SLBF | __SNBF) ? 0 : fp->_bf._size;

  while (n > 0)
 804c5fc:	85 db                	test   %ebx,%ebx
 804c5fe:	7e cc                	jle    804c5cc <__sflush_r+0xc4>
    {
      t = fp->_write (ptr, fp->_cookie, (char *) p, n);
 804c600:	89 5c 24 0c          	mov    %ebx,0xc(%esp)
 804c604:	89 6c 24 08          	mov    %ebp,0x8(%esp)
 804c608:	8b 46 1c             	mov    0x1c(%esi),%eax
 804c60b:	89 44 24 04          	mov    %eax,0x4(%esp)
 804c60f:	89 3c 24             	mov    %edi,(%esp)
 804c612:	ff 56 24             	call   *0x24(%esi)
      if (t <= 0)
 804c615:	85 c0                	test   %eax,%eax
 804c617:	7f df                	jg     804c5f8 <__sflush_r+0xf0>
	{
          fp->_flags |= __SERR;
 804c619:	66 83 4e 0c 40       	orw    $0x40,0xc(%esi)
          return EOF;
 804c61e:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
	}
      p += t;
      n -= t;
    }
  return 0;
}
 804c623:	83 c4 1c             	add    $0x1c,%esp
 804c626:	5b                   	pop    %ebx
 804c627:	5e                   	pop    %esi
 804c628:	5f                   	pop    %edi
 804c629:	5d                   	pop    %ebp
 804c62a:	c3                   	ret    
 804c62b:	90                   	nop
   * Set these immediately to avoid problems with longjmp
   * and to allow exchange buffering (via setvbuf) in user
   * write function.
   */
  fp->_p = p;
  fp->_w = flags & (__SLBF | __SNBF) ? 0 : fp->_bf._size;
 804c62c:	31 c0                	xor    %eax,%eax
 804c62e:	eb bc                	jmp    804c5ec <__sflush_r+0xe4>
         the next byte from the file rather than the buffer.  This conforms
         to the POSIX and SUSv3 standards.  Note that the standards allow
         this seek to be deferred until necessary, but we choose to do it here
         to make the change simpler, more contained, and less likely
         to miss a code scenario.  */
      if ((fp->_r > 0 || fp->_ur > 0) && fp->_seek != NULL)
 804c630:	8b 5e 3c             	mov    0x3c(%esi),%ebx
 804c633:	85 db                	test   %ebx,%ebx
 804c635:	0f 8f fb fe ff ff    	jg     804c536 <__sflush_r+0x2e>
 804c63b:	eb 8f                	jmp    804c5cc <__sflush_r+0xc4>
 804c63d:	8d 76 00             	lea    0x0(%esi),%esi
#ifdef __LARGE64_FILES
	      if (fp->_flags & __SL64)
		curoff = fp->_seek64 (ptr, fp->_cookie, 0, SEEK_CUR);
	      else
#endif
		curoff = fp->_seek (ptr, fp->_cookie, 0, SEEK_CUR);
 804c640:	c7 44 24 0c 01 00 00 	movl   $0x1,0xc(%esp)
 804c647:	00 
 804c648:	c7 44 24 08 00 00 00 	movl   $0x0,0x8(%esp)
 804c64f:	00 
 804c650:	8b 46 1c             	mov    0x1c(%esi),%eax
 804c653:	89 44 24 04          	mov    %eax,0x4(%esp)
 804c657:	89 3c 24             	mov    %edi,(%esp)
 804c65a:	ff d2                	call   *%edx
	      if (curoff == -1L && ptr->_errno != 0)
 804c65c:	83 f8 ff             	cmp    $0xffffffff,%eax
 804c65f:	74 37                	je     804c698 <__sflush_r+0x190>
 804c661:	8b 4e 0c             	mov    0xc(%esi),%ecx
 804c664:	8b 56 28             	mov    0x28(%esi),%edx
 804c667:	e9 e9 fe ff ff       	jmp    804c555 <__sflush_r+0x4d>
	  if (fp->_flags & __SL64)
	    curoff = fp->_seek64 (ptr, fp->_cookie, curoff, SEEK_SET);
	  else
#endif
	    curoff = fp->_seek (ptr, fp->_cookie, curoff, SEEK_SET);
	  if (curoff != -1 || ptr->_errno == 0
 804c66c:	8b 17                	mov    (%edi),%edx
 804c66e:	85 d2                	test   %edx,%edx
 804c670:	0f 84 12 ff ff ff    	je     804c588 <__sflush_r+0x80>
	      || ptr->_errno == ESPIPE || ptr->_errno == EINVAL)
 804c676:	83 fa 1d             	cmp    $0x1d,%edx
 804c679:	74 05                	je     804c680 <__sflush_r+0x178>
 804c67b:	83 fa 16             	cmp    $0x16,%edx
 804c67e:	75 99                	jne    804c619 <__sflush_r+0x111>
	    {
	      /* Seek successful or ignorable error condition.
		 We can clear read buffer now.  */
#ifdef _FSEEK_OPTIMIZATION
	      fp->_flags &= ~__SNPT;
 804c680:	66 81 66 0c ff f7    	andw   $0xf7ff,0xc(%esi)
#endif
	      fp->_r = 0;
 804c686:	c7 46 04 00 00 00 00 	movl   $0x0,0x4(%esi)
	      fp->_p = fp->_bf._base;
 804c68d:	8b 46 10             	mov    0x10(%esi),%eax
 804c690:	89 06                	mov    %eax,(%esi)
 804c692:	e9 12 ff ff ff       	jmp    804c5a9 <__sflush_r+0xa1>
 804c697:	90                   	nop
	      if (fp->_flags & __SL64)
		curoff = fp->_seek64 (ptr, fp->_cookie, 0, SEEK_CUR);
	      else
#endif
		curoff = fp->_seek (ptr, fp->_cookie, 0, SEEK_CUR);
	      if (curoff == -1L && ptr->_errno != 0)
 804c698:	8b 17                	mov    (%edi),%edx
 804c69a:	85 d2                	test   %edx,%edx
 804c69c:	74 c3                	je     804c661 <__sflush_r+0x159>
		{
		  int result = EOF;
		  if (ptr->_errno == ESPIPE || ptr->_errno == EINVAL)
 804c69e:	83 fa 1d             	cmp    $0x1d,%edx
 804c6a1:	74 09                	je     804c6ac <__sflush_r+0x1a4>
 804c6a3:	83 fa 16             	cmp    $0x16,%edx
 804c6a6:	0f 85 6d ff ff ff    	jne    804c619 <__sflush_r+0x111>
		    {
		      result = 0;
		      ptr->_errno = tmp_errno;
 804c6ac:	89 1f                	mov    %ebx,(%edi)
	      if (curoff == -1L && ptr->_errno != 0)
		{
		  int result = EOF;
		  if (ptr->_errno == ESPIPE || ptr->_errno == EINVAL)
		    {
		      result = 0;
 804c6ae:	31 c0                	xor    %eax,%eax
		      ptr->_errno = tmp_errno;
 804c6b0:	e9 19 ff ff ff       	jmp    804c5ce <__sflush_r+0xc6>
	      fp->_flags &= ~__SNPT;
#endif
	      fp->_r = 0;
	      fp->_p = fp->_bf._base;
	      if ((fp->_flags & __SOFF) && (curoff != -1 || ptr->_errno == 0))
		fp->_offset = curoff;
 804c6b5:	89 46 50             	mov    %eax,0x50(%esi)
 804c6b8:	e9 ec fe ff ff       	jmp    804c5a9 <__sflush_r+0xa1>
 804c6bd:	8d 76 00             	lea    0x0(%esi),%esi

0804c6c0 <_fflush_r>:

int
_DEFUN(_fflush_r, (ptr, fp),
       struct _reent *ptr _AND
       register FILE * fp)
{
 804c6c0:	56                   	push   %esi
 804c6c1:	53                   	push   %ebx
 804c6c2:	83 ec 14             	sub    $0x14,%esp
 804c6c5:	8b 5c 24 20          	mov    0x20(%esp),%ebx
 804c6c9:	8b 74 24 24          	mov    0x24(%esp),%esi
     2 is implemented here due to its simplicity.  */
  if (fp->_bf._base == NULL)
    return 0;
#endif /* _REENT_SMALL  */

  CHECK_INIT (ptr, fp);
 804c6cd:	85 db                	test   %ebx,%ebx
 804c6cf:	74 07                	je     804c6d8 <_fflush_r+0x18>
 804c6d1:	8b 43 38             	mov    0x38(%ebx),%eax
 804c6d4:	85 c0                	test   %eax,%eax
 804c6d6:	74 24                	je     804c6fc <_fflush_r+0x3c>

  if (!fp->_flags)
 804c6d8:	66 83 7e 0c 00       	cmpw   $0x0,0xc(%esi)
 804c6dd:	75 09                	jne    804c6e8 <_fflush_r+0x28>

  _newlib_flockfile_start (fp);
  ret = __sflush_r (ptr, fp);
  _newlib_flockfile_end (fp);
  return ret;
}
 804c6df:	31 c0                	xor    %eax,%eax
 804c6e1:	83 c4 14             	add    $0x14,%esp
 804c6e4:	5b                   	pop    %ebx
 804c6e5:	5e                   	pop    %esi
 804c6e6:	c3                   	ret    
 804c6e7:	90                   	nop

  if (!fp->_flags)
    return 0;

  _newlib_flockfile_start (fp);
  ret = __sflush_r (ptr, fp);
 804c6e8:	89 74 24 24          	mov    %esi,0x24(%esp)
 804c6ec:	89 5c 24 20          	mov    %ebx,0x20(%esp)
  _newlib_flockfile_end (fp);
  return ret;
}
 804c6f0:	83 c4 14             	add    $0x14,%esp
 804c6f3:	5b                   	pop    %ebx
 804c6f4:	5e                   	pop    %esi

  if (!fp->_flags)
    return 0;

  _newlib_flockfile_start (fp);
  ret = __sflush_r (ptr, fp);
 804c6f5:	e9 0e fe ff ff       	jmp    804c508 <__sflush_r>
 804c6fa:	66 90                	xchg   %ax,%ax
     2 is implemented here due to its simplicity.  */
  if (fp->_bf._base == NULL)
    return 0;
#endif /* _REENT_SMALL  */

  CHECK_INIT (ptr, fp);
 804c6fc:	89 1c 24             	mov    %ebx,(%esp)
 804c6ff:	e8 60 01 00 00       	call   804c864 <__sinit>
 804c704:	eb d2                	jmp    804c6d8 <_fflush_r+0x18>
 804c706:	66 90                	xchg   %ax,%ax

0804c708 <fflush>:
#ifndef _REENT_ONLY

int
_DEFUN(fflush, (fp),
       register FILE * fp)
{
 804c708:	83 ec 1c             	sub    $0x1c,%esp
 804c70b:	8b 44 24 20          	mov    0x20(%esp),%eax
  if (fp == NULL)
 804c70f:	85 c0                	test   %eax,%eax
 804c711:	74 15                	je     804c728 <fflush+0x20>
    return _fwalk_reent (_GLOBAL_REENT, _fflush_r);

  return _fflush_r (_REENT, fp);
 804c713:	89 44 24 04          	mov    %eax,0x4(%esp)
 804c717:	a1 40 40 05 08       	mov    0x8054040,%eax
 804c71c:	89 04 24             	mov    %eax,(%esp)
 804c71f:	e8 9c ff ff ff       	call   804c6c0 <_fflush_r>
}
 804c724:	83 c4 1c             	add    $0x1c,%esp
 804c727:	c3                   	ret    
int
_DEFUN(fflush, (fp),
       register FILE * fp)
{
  if (fp == NULL)
    return _fwalk_reent (_GLOBAL_REENT, _fflush_r);
 804c728:	c7 44 24 04 c0 c6 04 	movl   $0x804c6c0,0x4(%esp)
 804c72f:	08 
 804c730:	a1 8c 16 05 08       	mov    0x805168c,%eax
 804c735:	89 04 24             	mov    %eax,(%esp)
 804c738:	e8 07 0b 00 00       	call   804d244 <_fwalk_reent>

  return _fflush_r (_REENT, fp);
}
 804c73d:	83 c4 1c             	add    $0x1c,%esp
 804c740:	c3                   	ret    
 804c741:	90                   	nop
 804c742:	90                   	nop
 804c743:	90                   	nop

0804c744 <__fp_lock>:
       FILE * ptr)
{
  _flockfile (ptr);

  return 0;
}
 804c744:	31 c0                	xor    %eax,%eax
 804c746:	c3                   	ret    
 804c747:	90                   	nop

0804c748 <__fp_unlock>:
       FILE * ptr)
{
  _funlockfile (ptr);

  return 0;
}
 804c748:	31 c0                	xor    %eax,%eax
 804c74a:	c3                   	ret    
 804c74b:	90                   	nop

0804c74c <_cleanup_r>:
 */

_VOID
_DEFUN(_cleanup_r, (ptr),
       struct _reent *ptr)
{
 804c74c:	83 ec 1c             	sub    $0x1c,%esp
  _CAST_VOID _fwalk(ptr, fclose);
 804c74f:	c7 44 24 04 d4 04 05 	movl   $0x80504d4,0x4(%esp)
 804c756:	08 
 804c757:	8b 44 24 20          	mov    0x20(%esp),%eax
 804c75b:	89 04 24             	mov    %eax,(%esp)
 804c75e:	e8 89 0a 00 00       	call   804d1ec <_fwalk>
  /* _CAST_VOID _fwalk (ptr, fflush); */	/* `cheating' */
}
 804c763:	83 c4 1c             	add    $0x1c,%esp
 804c766:	c3                   	ret    
 804c767:	90                   	nop

0804c768 <std.isra.0>:
const struct __sFILE_fake __sf_fake_stderr =
    {_NULL, 0, 0, 0, 0, {_NULL, 0}, 0, _NULL};
#endif

static _VOID
_DEFUN(std, (ptr, flags, file, data),
 804c768:	53                   	push   %ebx
 804c769:	83 ec 18             	sub    $0x18,%esp
 804c76c:	89 c3                	mov    %eax,%ebx
            FILE *ptr _AND
            int flags _AND
            int file  _AND
            struct _reent *data)
{
  ptr->_p = 0;
 804c76e:	c7 00 00 00 00 00    	movl   $0x0,(%eax)
  ptr->_r = 0;
 804c774:	c7 40 04 00 00 00 00 	movl   $0x0,0x4(%eax)
  ptr->_w = 0;
 804c77b:	c7 40 08 00 00 00 00 	movl   $0x0,0x8(%eax)
  ptr->_flags = flags;
 804c782:	66 89 50 0c          	mov    %dx,0xc(%eax)
  ptr->_flags2 = 0;
 804c786:	c7 40 64 00 00 00 00 	movl   $0x0,0x64(%eax)
  ptr->_file = file;
 804c78d:	66 89 48 0e          	mov    %cx,0xe(%eax)
  ptr->_bf._base = 0;
 804c791:	c7 40 10 00 00 00 00 	movl   $0x0,0x10(%eax)
  ptr->_bf._size = 0;
 804c798:	c7 40 14 00 00 00 00 	movl   $0x0,0x14(%eax)
  ptr->_lbfsize = 0;
 804c79f:	c7 40 18 00 00 00 00 	movl   $0x0,0x18(%eax)
  memset (&ptr->_mbstate, 0, sizeof (_mbstate_t));
 804c7a6:	c7 44 24 08 08 00 00 	movl   $0x8,0x8(%esp)
 804c7ad:	00 
 804c7ae:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
 804c7b5:	00 
 804c7b6:	8d 40 5c             	lea    0x5c(%eax),%eax
 804c7b9:	89 04 24             	mov    %eax,(%esp)
 804c7bc:	e8 5f 15 00 00       	call   804dd20 <memset>
  ptr->_cookie = ptr;
 804c7c1:	89 5b 1c             	mov    %ebx,0x1c(%ebx)
  ptr->_read = __sread;
 804c7c4:	c7 43 20 80 f1 04 08 	movl   $0x804f180,0x20(%ebx)
#ifndef __LARGE64_FILES
  ptr->_write = __swrite;
 804c7cb:	c7 43 24 c8 f1 04 08 	movl   $0x804f1c8,0x24(%ebx)
#else /* __LARGE64_FILES */
  ptr->_write = __swrite64;
  ptr->_seek64 = __sseek64;
  ptr->_flags |= __SL64;
#endif /* __LARGE64_FILES */
  ptr->_seek = __sseek;
 804c7d2:	c7 43 28 34 f2 04 08 	movl   $0x804f234,0x28(%ebx)
#ifdef _STDIO_CLOSE_PER_REENT_STD_STREAMS
  ptr->_close = __sclose;
 804c7d9:	c7 43 2c 80 f2 04 08 	movl   $0x804f280,0x2c(%ebx)

#ifdef __SCLE
  if (__stextmode (ptr->_file))
    ptr->_flags |= __SCLE;
#endif
}
 804c7e0:	83 c4 18             	add    $0x18,%esp
 804c7e3:	5b                   	pop    %ebx
 804c7e4:	c3                   	ret    
 804c7e5:	8d 76 00             	lea    0x0(%esi),%esi

0804c7e8 <__sfmoreglue>:

struct _glue *
_DEFUN(__sfmoreglue, (d, n),
       struct _reent *d _AND
       register int n)
{
 804c7e8:	56                   	push   %esi
 804c7e9:	53                   	push   %ebx
 804c7ea:	83 ec 14             	sub    $0x14,%esp
 804c7ed:	8b 74 24 24          	mov    0x24(%esp),%esi
  struct glue_with_file *g;

  g = (struct glue_with_file *)
    _malloc_r (d, sizeof (*g) + (n - 1) * sizeof (FILE));
 804c7f1:	8d 46 ff             	lea    -0x1(%esi),%eax
 804c7f4:	8d 14 40             	lea    (%eax,%eax,2),%edx
 804c7f7:	8d 04 90             	lea    (%eax,%edx,4),%eax
 804c7fa:	8d 04 c5 74 00 00 00 	lea    0x74(,%eax,8),%eax
       struct _reent *d _AND
       register int n)
{
  struct glue_with_file *g;

  g = (struct glue_with_file *)
 804c801:	89 44 24 04          	mov    %eax,0x4(%esp)
 804c805:	8b 44 24 20          	mov    0x20(%esp),%eax
 804c809:	89 04 24             	mov    %eax,(%esp)
 804c80c:	e8 ab 0c 00 00       	call   804d4bc <_malloc_r>
 804c811:	89 c3                	mov    %eax,%ebx
    _malloc_r (d, sizeof (*g) + (n - 1) * sizeof (FILE));
  if (g == NULL)
 804c813:	85 c0                	test   %eax,%eax
 804c815:	74 35                	je     804c84c <__sfmoreglue+0x64>
    return NULL;
  g->glue._next = NULL;
 804c817:	c7 00 00 00 00 00    	movl   $0x0,(%eax)
  g->glue._niobs = n;
 804c81d:	89 70 04             	mov    %esi,0x4(%eax)
  g->glue._iobs = &g->file;
 804c820:	8d 40 0c             	lea    0xc(%eax),%eax
 804c823:	89 43 08             	mov    %eax,0x8(%ebx)
  memset (&g->file, 0, n * sizeof (FILE));
 804c826:	8d 14 76             	lea    (%esi,%esi,2),%edx
 804c829:	8d 14 96             	lea    (%esi,%edx,4),%edx
 804c82c:	c1 e2 03             	shl    $0x3,%edx
 804c82f:	89 54 24 08          	mov    %edx,0x8(%esp)
 804c833:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
 804c83a:	00 
 804c83b:	89 04 24             	mov    %eax,(%esp)
 804c83e:	e8 dd 14 00 00       	call   804dd20 <memset>
  return &g->glue;
 804c843:	89 d8                	mov    %ebx,%eax
}
 804c845:	83 c4 14             	add    $0x14,%esp
 804c848:	5b                   	pop    %ebx
 804c849:	5e                   	pop    %esi
 804c84a:	c3                   	ret    
 804c84b:	90                   	nop
  struct glue_with_file *g;

  g = (struct glue_with_file *)
    _malloc_r (d, sizeof (*g) + (n - 1) * sizeof (FILE));
  if (g == NULL)
    return NULL;
 804c84c:	31 c0                	xor    %eax,%eax
 804c84e:	eb f5                	jmp    804c845 <__sfmoreglue+0x5d>

0804c850 <_cleanup>:
}

#ifndef _REENT_ONLY
_VOID
_DEFUN_VOID(_cleanup)
{
 804c850:	83 ec 1c             	sub    $0x1c,%esp
  _cleanup_r (_GLOBAL_REENT);
 804c853:	a1 8c 16 05 08       	mov    0x805168c,%eax
 804c858:	89 04 24             	mov    %eax,(%esp)
 804c85b:	e8 ec fe ff ff       	call   804c74c <_cleanup_r>
}
 804c860:	83 c4 1c             	add    $0x1c,%esp
 804c863:	c3                   	ret    

0804c864 <__sinit>:
 */

_VOID
_DEFUN(__sinit, (s),
       struct _reent *s)
{
 804c864:	53                   	push   %ebx
 804c865:	83 ec 08             	sub    $0x8,%esp
 804c868:	8b 5c 24 10          	mov    0x10(%esp),%ebx
  __sinit_lock_acquire ();

  if (s->__sdidinit)
 804c86c:	8b 43 38             	mov    0x38(%ebx),%eax
 804c86f:	85 c0                	test   %eax,%eax
 804c871:	75 61                	jne    804c8d4 <__sinit+0x70>
      __sinit_lock_release ();
      return;
    }

  /* make sure we clean up on exit */
  s->__cleanup = _cleanup_r;	/* conservative */
 804c873:	c7 43 3c 4c c7 04 08 	movl   $0x804c74c,0x3c(%ebx)

  s->__sglue._next = NULL;
 804c87a:	c7 83 e0 02 00 00 00 	movl   $0x0,0x2e0(%ebx)
 804c881:	00 00 00 
#ifndef _REENT_SMALL
  s->__sglue._niobs = 3;
 804c884:	c7 83 e4 02 00 00 03 	movl   $0x3,0x2e4(%ebx)
 804c88b:	00 00 00 
  s->__sglue._iobs = &s->__sf[0];
 804c88e:	8d 83 ec 02 00 00    	lea    0x2ec(%ebx),%eax
 804c894:	89 83 e8 02 00 00    	mov    %eax,0x2e8(%ebx)
  s->_stdin = __sfp(s);
  s->_stdout = __sfp(s);
  s->_stderr = __sfp(s);
#endif

  std (s->_stdin,  __SRD, 0, s);
 804c89a:	8b 43 04             	mov    0x4(%ebx),%eax
 804c89d:	31 c9                	xor    %ecx,%ecx
 804c89f:	ba 04 00 00 00       	mov    $0x4,%edx
 804c8a4:	e8 bf fe ff ff       	call   804c768 <std.isra.0>
     requires both stdin and stdout to be line-buffered, but tradition
     leaves stdin alone on systems without fcntl.  */
#ifdef HAVE_FCNTL
  std (s->_stdout, __SWR, 1, s);
#else
  std (s->_stdout, __SWR | __SLBF, 1, s);
 804c8a9:	8b 43 08             	mov    0x8(%ebx),%eax
 804c8ac:	b9 01 00 00 00       	mov    $0x1,%ecx
 804c8b1:	ba 09 00 00 00       	mov    $0x9,%edx
 804c8b6:	e8 ad fe ff ff       	call   804c768 <std.isra.0>
#endif

  /* POSIX requires stderr to be opened for reading and writing, even
     when the underlying fd 2 is write-only.  */
  std (s->_stderr, __SRW | __SNBF, 2, s);
 804c8bb:	8b 43 0c             	mov    0xc(%ebx),%eax
 804c8be:	b9 02 00 00 00       	mov    $0x2,%ecx
 804c8c3:	ba 12 00 00 00       	mov    $0x12,%edx
 804c8c8:	e8 9b fe ff ff       	call   804c768 <std.isra.0>

  s->__sdidinit = 1;
 804c8cd:	c7 43 38 01 00 00 00 	movl   $0x1,0x38(%ebx)

  __sinit_lock_release ();
}
 804c8d4:	83 c4 08             	add    $0x8,%esp
 804c8d7:	5b                   	pop    %ebx
 804c8d8:	c3                   	ret    
 804c8d9:	8d 76 00             	lea    0x0(%esi),%esi

0804c8dc <__sfp>:
 */

FILE *
_DEFUN(__sfp, (d),
       struct _reent *d)
{
 804c8dc:	56                   	push   %esi
 804c8dd:	53                   	push   %ebx
 804c8de:	83 ec 14             	sub    $0x14,%esp
  int n;
  struct _glue *g;

  _newlib_sfp_lock_start ();

  if (!_GLOBAL_REENT->__sdidinit)
 804c8e1:	8b 35 8c 16 05 08    	mov    0x805168c,%esi
 804c8e7:	8b 56 38             	mov    0x38(%esi),%edx
 804c8ea:	85 d2                	test   %edx,%edx
 804c8ec:	0f 84 b6 00 00 00    	je     804c9a8 <__sfp+0xcc>
    __sinit (_GLOBAL_REENT);
  for (g = &_GLOBAL_REENT->__sglue;; g = g->_next)
 804c8f2:	81 c6 e0 02 00 00    	add    $0x2e0,%esi
    {
      for (fp = g->_iobs, n = g->_niobs; --n >= 0; fp++)
 804c8f8:	8b 5e 08             	mov    0x8(%esi),%ebx
 804c8fb:	8b 46 04             	mov    0x4(%esi),%eax
 804c8fe:	48                   	dec    %eax
 804c8ff:	79 14                	jns    804c915 <__sfp+0x39>
 804c901:	e9 92 00 00 00       	jmp    804c998 <__sfp+0xbc>
 804c906:	66 90                	xchg   %ax,%ax
 804c908:	83 c3 68             	add    $0x68,%ebx
 804c90b:	48                   	dec    %eax
 804c90c:	83 f8 ff             	cmp    $0xffffffff,%eax
 804c90f:	0f 84 83 00 00 00    	je     804c998 <__sfp+0xbc>
	if (fp->_flags == 0)
 804c915:	66 83 7b 0c 00       	cmpw   $0x0,0xc(%ebx)
 804c91a:	75 ec                	jne    804c908 <__sfp+0x2c>
  _newlib_sfp_lock_exit ();
  d->_errno = ENOMEM;
  return NULL;

found:
  fp->_file = -1;		/* no file */
 804c91c:	66 c7 43 0e ff ff    	movw   $0xffff,0xe(%ebx)
  fp->_flags = 1;		/* reserve this slot; caller sets real flags */
 804c922:	66 c7 43 0c 01 00    	movw   $0x1,0xc(%ebx)
  fp->_flags2 = 0;
 804c928:	c7 43 64 00 00 00 00 	movl   $0x0,0x64(%ebx)
#ifndef __SINGLE_THREAD__
  __lock_init_recursive (fp->_lock);
#endif
  _newlib_sfp_lock_end ();

  fp->_p = NULL;		/* no current pointer */
 804c92f:	c7 03 00 00 00 00    	movl   $0x0,(%ebx)
  fp->_w = 0;			/* nothing to read or write */
 804c935:	c7 43 08 00 00 00 00 	movl   $0x0,0x8(%ebx)
  fp->_r = 0;
 804c93c:	c7 43 04 00 00 00 00 	movl   $0x0,0x4(%ebx)
  fp->_bf._base = NULL;		/* no buffer */
 804c943:	c7 43 10 00 00 00 00 	movl   $0x0,0x10(%ebx)
  fp->_bf._size = 0;
 804c94a:	c7 43 14 00 00 00 00 	movl   $0x0,0x14(%ebx)
  fp->_lbfsize = 0;		/* not line buffered */
 804c951:	c7 43 18 00 00 00 00 	movl   $0x0,0x18(%ebx)
  memset (&fp->_mbstate, 0, sizeof (_mbstate_t));
 804c958:	c7 44 24 08 08 00 00 	movl   $0x8,0x8(%esp)
 804c95f:	00 
 804c960:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
 804c967:	00 
 804c968:	8d 43 5c             	lea    0x5c(%ebx),%eax
 804c96b:	89 04 24             	mov    %eax,(%esp)
 804c96e:	e8 ad 13 00 00       	call   804dd20 <memset>
  /* fp->_cookie = <any>; */	/* caller sets cookie, _read/_write etc */
  fp->_ub._base = NULL;		/* no ungetc buffer */
 804c973:	c7 43 30 00 00 00 00 	movl   $0x0,0x30(%ebx)
  fp->_ub._size = 0;
 804c97a:	c7 43 34 00 00 00 00 	movl   $0x0,0x34(%ebx)
  fp->_lb._base = NULL;		/* no line buffer */
 804c981:	c7 43 44 00 00 00 00 	movl   $0x0,0x44(%ebx)
  fp->_lb._size = 0;
 804c988:	c7 43 48 00 00 00 00 	movl   $0x0,0x48(%ebx)

  return fp;
}
 804c98f:	89 d8                	mov    %ebx,%eax
 804c991:	83 c4 14             	add    $0x14,%esp
 804c994:	5b                   	pop    %ebx
 804c995:	5e                   	pop    %esi
 804c996:	c3                   	ret    
 804c997:	90                   	nop
  for (g = &_GLOBAL_REENT->__sglue;; g = g->_next)
    {
      for (fp = g->_iobs, n = g->_niobs; --n >= 0; fp++)
	if (fp->_flags == 0)
	  goto found;
      if (g->_next == NULL &&
 804c998:	8b 06                	mov    (%esi),%eax
 804c99a:	85 c0                	test   %eax,%eax
 804c99c:	74 17                	je     804c9b5 <__sfp+0xd9>

  _newlib_sfp_lock_start ();

  if (!_GLOBAL_REENT->__sdidinit)
    __sinit (_GLOBAL_REENT);
  for (g = &_GLOBAL_REENT->__sglue;; g = g->_next)
 804c99e:	89 c6                	mov    %eax,%esi
	if (fp->_flags == 0)
	  goto found;
      if (g->_next == NULL &&
	  (g->_next = __sfmoreglue (d, NDYNAMIC)) == NULL)
	break;
    }
 804c9a0:	e9 53 ff ff ff       	jmp    804c8f8 <__sfp+0x1c>
 804c9a5:	8d 76 00             	lea    0x0(%esi),%esi
  struct _glue *g;

  _newlib_sfp_lock_start ();

  if (!_GLOBAL_REENT->__sdidinit)
    __sinit (_GLOBAL_REENT);
 804c9a8:	89 34 24             	mov    %esi,(%esp)
 804c9ab:	e8 b4 fe ff ff       	call   804c864 <__sinit>
 804c9b0:	e9 3d ff ff ff       	jmp    804c8f2 <__sfp+0x16>
    {
      for (fp = g->_iobs, n = g->_niobs; --n >= 0; fp++)
	if (fp->_flags == 0)
	  goto found;
      if (g->_next == NULL &&
	  (g->_next = __sfmoreglue (d, NDYNAMIC)) == NULL)
 804c9b5:	c7 44 24 04 04 00 00 	movl   $0x4,0x4(%esp)
 804c9bc:	00 
 804c9bd:	8b 44 24 20          	mov    0x20(%esp),%eax
 804c9c1:	89 04 24             	mov    %eax,(%esp)
 804c9c4:	e8 1f fe ff ff       	call   804c7e8 <__sfmoreglue>
 804c9c9:	89 06                	mov    %eax,(%esi)
  for (g = &_GLOBAL_REENT->__sglue;; g = g->_next)
    {
      for (fp = g->_iobs, n = g->_niobs; --n >= 0; fp++)
	if (fp->_flags == 0)
	  goto found;
      if (g->_next == NULL &&
 804c9cb:	85 c0                	test   %eax,%eax
 804c9cd:	75 cf                	jne    804c99e <__sfp+0xc2>
	  (g->_next = __sfmoreglue (d, NDYNAMIC)) == NULL)
	break;
    }
  _newlib_sfp_lock_exit ();
  d->_errno = ENOMEM;
 804c9cf:	8b 44 24 20          	mov    0x20(%esp),%eax
 804c9d3:	c7 00 0c 00 00 00    	movl   $0xc,(%eax)
  return NULL;
 804c9d9:	31 db                	xor    %ebx,%ebx
 804c9db:	eb b2                	jmp    804c98f <__sfp+0xb3>
 804c9dd:	8d 76 00             	lea    0x0(%esi),%esi

0804c9e0 <__sfp_lock_acquire>:
__LOCK_INIT_RECURSIVE(static, __sfp_lock);
__LOCK_INIT_RECURSIVE(static, __sinit_lock);

_VOID
_DEFUN_VOID(__sfp_lock_acquire)
{
 804c9e0:	c3                   	ret    
 804c9e1:	8d 76 00             	lea    0x0(%esi),%esi

0804c9e4 <__sfp_lock_release>:
  __lock_acquire_recursive (__sfp_lock);
}

_VOID
_DEFUN_VOID(__sfp_lock_release)
{
 804c9e4:	c3                   	ret    
 804c9e5:	8d 76 00             	lea    0x0(%esi),%esi

0804c9e8 <__sinit_lock_acquire>:
  __lock_release_recursive (__sfp_lock);
}

_VOID
_DEFUN_VOID(__sinit_lock_acquire)
{
 804c9e8:	c3                   	ret    
 804c9e9:	8d 76 00             	lea    0x0(%esi),%esi

0804c9ec <__sinit_lock_release>:
  __lock_acquire_recursive (__sinit_lock);
}

_VOID
_DEFUN_VOID(__sinit_lock_release)
{
 804c9ec:	c3                   	ret    
 804c9ed:	8d 76 00             	lea    0x0(%esi),%esi

0804c9f0 <__fp_lock_all>:
  return 0;
}

_VOID
_DEFUN_VOID(__fp_lock_all)
{
 804c9f0:	83 ec 1c             	sub    $0x1c,%esp
  __sfp_lock_acquire ();

  _CAST_VOID _fwalk (_REENT, __fp_lock);
 804c9f3:	c7 44 24 04 44 c7 04 	movl   $0x804c744,0x4(%esp)
 804c9fa:	08 
 804c9fb:	a1 40 40 05 08       	mov    0x8054040,%eax
 804ca00:	89 04 24             	mov    %eax,(%esp)
 804ca03:	e8 e4 07 00 00       	call   804d1ec <_fwalk>
}
 804ca08:	83 c4 1c             	add    $0x1c,%esp
 804ca0b:	c3                   	ret    

0804ca0c <__fp_unlock_all>:

_VOID
_DEFUN_VOID(__fp_unlock_all)
{
 804ca0c:	83 ec 1c             	sub    $0x1c,%esp
  _CAST_VOID _fwalk (_REENT, __fp_unlock);
 804ca0f:	c7 44 24 04 48 c7 04 	movl   $0x804c748,0x4(%esp)
 804ca16:	08 
 804ca17:	a1 40 40 05 08       	mov    0x8054040,%eax
 804ca1c:	89 04 24             	mov    %eax,(%esp)
 804ca1f:	e8 c8 07 00 00       	call   804d1ec <_fwalk>

  __sfp_lock_release ();
}
 804ca24:	83 c4 1c             	add    $0x1c,%esp
 804ca27:	c3                   	ret    

0804ca28 <_malloc_trim_r>:
#if __STD_C
int malloc_trim(RARG size_t pad)
#else
int malloc_trim(RARG pad) RDECL size_t pad;
#endif
{
 804ca28:	57                   	push   %edi
 804ca29:	56                   	push   %esi
 804ca2a:	53                   	push   %ebx
 804ca2b:	83 ec 10             	sub    $0x10,%esp
 804ca2e:	8b 74 24 20          	mov    0x20(%esp),%esi
  char* current_brk;     /* address returned by pre-check sbrk call */
  char* new_brk;         /* address returned by negative sbrk call */

  unsigned long pagesz = malloc_getpagesize;

  MALLOC_LOCK;
 804ca32:	89 34 24             	mov    %esi,(%esp)
 804ca35:	e8 6e 13 00 00       	call   804dda8 <__malloc_lock>

  top_size = chunksize(top);
 804ca3a:	a1 68 45 05 08       	mov    0x8054568,%eax
 804ca3f:	8b 58 04             	mov    0x4(%eax),%ebx
 804ca42:	83 e3 fc             	and    $0xfffffffc,%ebx
  extra = ((top_size - pad - MINSIZE + (pagesz-1)) / pagesz - 1) * pagesz;
 804ca45:	8d 83 ef 0f 00 00    	lea    0xfef(%ebx),%eax
 804ca4b:	2b 44 24 24          	sub    0x24(%esp),%eax
 804ca4f:	25 00 f0 ff ff       	and    $0xfffff000,%eax
 804ca54:	8d b8 00 f0 ff ff    	lea    -0x1000(%eax),%edi

  if (extra < (long)pagesz)  /* Not enough memory to release */
 804ca5a:	81 ff ff 0f 00 00    	cmp    $0xfff,%edi
 804ca60:	7e 1c                	jle    804ca7e <_malloc_trim_r+0x56>
  }

  else
  {
    /* Test to make sure no one else called sbrk */
    current_brk = (char*)(MORECORE (0));
 804ca62:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
 804ca69:	00 
 804ca6a:	89 34 24             	mov    %esi,(%esp)
 804ca6d:	e8 5a 26 00 00       	call   804f0cc <_sbrk_r>
    if (current_brk != (char*)(top) + top_size)
 804ca72:	8b 15 68 45 05 08    	mov    0x8054568,%edx
 804ca78:	01 da                	add    %ebx,%edx
 804ca7a:	39 d0                	cmp    %edx,%eax
 804ca7c:	74 12                	je     804ca90 <_malloc_trim_r+0x68>
  top_size = chunksize(top);
  extra = ((top_size - pad - MINSIZE + (pagesz-1)) / pagesz - 1) * pagesz;

  if (extra < (long)pagesz)  /* Not enough memory to release */
  {
    MALLOC_UNLOCK;
 804ca7e:	89 34 24             	mov    %esi,(%esp)
 804ca81:	e8 26 13 00 00       	call   804ddac <__malloc_unlock>
    return 0;
 804ca86:	31 c0                	xor    %eax,%eax
	MALLOC_UNLOCK;
        return 1;
      }
    }
  }
}
 804ca88:	83 c4 10             	add    $0x10,%esp
 804ca8b:	5b                   	pop    %ebx
 804ca8c:	5e                   	pop    %esi
 804ca8d:	5f                   	pop    %edi
 804ca8e:	c3                   	ret    
 804ca8f:	90                   	nop
      return 0;     /* Apparently we don't own memory; must fail */
    }

    else
    {
      new_brk = (char*)(MORECORE (-extra));
 804ca90:	89 f8                	mov    %edi,%eax
 804ca92:	f7 d8                	neg    %eax
 804ca94:	89 44 24 04          	mov    %eax,0x4(%esp)
 804ca98:	89 34 24             	mov    %esi,(%esp)
 804ca9b:	e8 2c 26 00 00       	call   804f0cc <_sbrk_r>
      
      if (new_brk == (char*)(MORECORE_FAILURE)) /* sbrk failed? */
 804caa0:	40                   	inc    %eax
 804caa1:	74 29                	je     804cacc <_malloc_trim_r+0xa4>
      }

      else
      {
        /* Success. Adjust top accordingly. */
        set_head(top, (top_size - extra) | PREV_INUSE);
 804caa3:	29 fb                	sub    %edi,%ebx
 804caa5:	83 cb 01             	or     $0x1,%ebx
 804caa8:	a1 68 45 05 08       	mov    0x8054568,%eax
 804caad:	89 58 04             	mov    %ebx,0x4(%eax)
        sbrked_mem -= extra;
 804cab0:	29 3d c0 49 05 08    	sub    %edi,0x80549c0
        check_chunk(top);
	MALLOC_UNLOCK;
 804cab6:	89 34 24             	mov    %esi,(%esp)
 804cab9:	e8 ee 12 00 00       	call   804ddac <__malloc_unlock>
        return 1;
 804cabe:	b8 01 00 00 00       	mov    $0x1,%eax
      }
    }
  }
}
 804cac3:	83 c4 10             	add    $0x10,%esp
 804cac6:	5b                   	pop    %ebx
 804cac7:	5e                   	pop    %esi
 804cac8:	5f                   	pop    %edi
 804cac9:	c3                   	ret    
 804caca:	66 90                	xchg   %ax,%ax
      new_brk = (char*)(MORECORE (-extra));
      
      if (new_brk == (char*)(MORECORE_FAILURE)) /* sbrk failed? */
      {
        /* Try to figure out what we have */
        current_brk = (char*)(MORECORE (0));
 804cacc:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
 804cad3:	00 
 804cad4:	89 34 24             	mov    %esi,(%esp)
 804cad7:	e8 f0 25 00 00       	call   804f0cc <_sbrk_r>
        top_size = current_brk - (char*)top;
 804cadc:	8b 15 68 45 05 08    	mov    0x8054568,%edx
 804cae2:	89 c1                	mov    %eax,%ecx
 804cae4:	29 d1                	sub    %edx,%ecx
        if (top_size >= (long)MINSIZE) /* if not, we are very very dead! */
 804cae6:	83 f9 0f             	cmp    $0xf,%ecx
 804cae9:	7e 93                	jle    804ca7e <_malloc_trim_r+0x56>
        {
          sbrked_mem = current_brk - sbrk_base;
 804caeb:	2b 05 40 45 05 08    	sub    0x8054540,%eax
 804caf1:	a3 c0 49 05 08       	mov    %eax,0x80549c0
          set_head(top, top_size | PREV_INUSE);
 804caf6:	83 c9 01             	or     $0x1,%ecx
 804caf9:	89 4a 04             	mov    %ecx,0x4(%edx)
 804cafc:	eb 80                	jmp    804ca7e <_malloc_trim_r+0x56>
 804cafe:	66 90                	xchg   %ax,%ax

0804cb00 <_free_r>:
#if __STD_C
void fREe(RARG Void_t* mem)
#else
void fREe(RARG mem) RDECL Void_t* mem;
#endif
{
 804cb00:	55                   	push   %ebp
 804cb01:	57                   	push   %edi
 804cb02:	56                   	push   %esi
 804cb03:	53                   	push   %ebx
 804cb04:	83 ec 1c             	sub    $0x1c,%esp
 804cb07:	8b 74 24 30          	mov    0x30(%esp),%esi
 804cb0b:	8b 7c 24 34          	mov    0x34(%esp),%edi
  INTERNAL_SIZE_T prevsz; /* size of previous contiguous chunk */
  mchunkptr bck;       /* misc temp for linking */
  mchunkptr fwd;       /* misc temp for linking */
  int       islr;      /* track whether merging with last_remainder */

  if (mem == 0)                              /* free(0) has no effect */
 804cb0f:	85 ff                	test   %edi,%edi
 804cb11:	0f 84 f1 00 00 00    	je     804cc08 <_free_r+0x108>
    return;

  MALLOC_LOCK;
 804cb17:	89 34 24             	mov    %esi,(%esp)
 804cb1a:	e8 89 12 00 00       	call   804dda8 <__malloc_lock>

  p = mem2chunk(mem);
 804cb1f:	8d 5f f8             	lea    -0x8(%edi),%ebx
  hd = p->size;
 804cb22:	8b 6f fc             	mov    -0x4(%edi),%ebp
  }
#endif
  
  check_inuse_chunk(p);
  
  sz = hd & ~PREV_INUSE;
 804cb25:	89 e8                	mov    %ebp,%eax
 804cb27:	83 e0 fe             	and    $0xfffffffe,%eax
  next = chunk_at_offset(p, sz);
 804cb2a:	8d 14 03             	lea    (%ebx,%eax,1),%edx
  nextsz = chunksize(next);
 804cb2d:	8b 4a 04             	mov    0x4(%edx),%ecx
 804cb30:	83 e1 fc             	and    $0xfffffffc,%ecx
  
  if (next == top)                            /* merge with top */
 804cb33:	39 15 68 45 05 08    	cmp    %edx,0x8054568
 804cb39:	0f 84 45 01 00 00    	je     804cc84 <_free_r+0x184>
      malloc_trim(RCALL top_pad); 
    MALLOC_UNLOCK;
    return;
  }

  set_head(next, nextsz);                    /* clear inuse bit */
 804cb3f:	89 4a 04             	mov    %ecx,0x4(%edx)

  islr = 0;

  if (!(hd & PREV_INUSE))                    /* consolidate backward */
 804cb42:	83 e5 01             	and    $0x1,%ebp
 804cb45:	0f 84 95 00 00 00    	je     804cbe0 <_free_r+0xe0>
    return;
  }

  set_head(next, nextsz);                    /* clear inuse bit */

  islr = 0;
 804cb4b:	31 ff                	xor    %edi,%edi
      islr = 1;
    else
      unlink(p, bck, fwd);
  }
  
  if (!(inuse_bit_at_offset(next, nextsz)))   /* consolidate forward */
 804cb4d:	f6 44 0a 04 01       	testb  $0x1,0x4(%edx,%ecx,1)
 804cb52:	75 16                	jne    804cb6a <_free_r+0x6a>
  {
    sz += nextsz;
 804cb54:	01 c8                	add    %ecx,%eax
    
    if (!islr && next->fd == last_remainder)  /* re-insert last_remainder */
 804cb56:	8b 4a 08             	mov    0x8(%edx),%ecx
 804cb59:	85 ff                	test   %edi,%edi
 804cb5b:	0f 84 eb 00 00 00    	je     804cc4c <_free_r+0x14c>
    {
      islr = 1;
      link_last_remainder(p);   
    }
    else
      unlink(next, bck, fwd);
 804cb61:	8b 52 0c             	mov    0xc(%edx),%edx
 804cb64:	89 51 0c             	mov    %edx,0xc(%ecx)
 804cb67:	89 4a 08             	mov    %ecx,0x8(%edx)
  }


  set_head(p, sz | PREV_INUSE);
 804cb6a:	89 c2                	mov    %eax,%edx
 804cb6c:	83 ca 01             	or     $0x1,%edx
 804cb6f:	89 53 04             	mov    %edx,0x4(%ebx)
  set_foot(p, sz);
 804cb72:	89 04 03             	mov    %eax,(%ebx,%eax,1)
  if (!islr)
 804cb75:	85 ff                	test   %edi,%edi
 804cb77:	75 57                	jne    804cbd0 <_free_r+0xd0>
    frontlink(p, sz, idx, bck, fwd);  
 804cb79:	3d ff 01 00 00       	cmp    $0x1ff,%eax
 804cb7e:	0f 86 8c 00 00 00    	jbe    804cc10 <_free_r+0x110>
 804cb84:	89 c1                	mov    %eax,%ecx
 804cb86:	c1 e9 09             	shr    $0x9,%ecx
 804cb89:	83 f9 04             	cmp    $0x4,%ecx
 804cb8c:	0f 87 4a 01 00 00    	ja     804ccdc <_free_r+0x1dc>
 804cb92:	89 c1                	mov    %eax,%ecx
 804cb94:	c1 e9 06             	shr    $0x6,%ecx
 804cb97:	83 c1 38             	add    $0x38,%ecx
 804cb9a:	8d 14 09             	lea    (%ecx,%ecx,1),%edx
 804cb9d:	8d 3c 95 60 45 05 08 	lea    0x8054560(,%edx,4),%edi
 804cba4:	8b 57 08             	mov    0x8(%edi),%edx
 804cba7:	39 fa                	cmp    %edi,%edx
 804cba9:	0f 84 3d 01 00 00    	je     804ccec <_free_r+0x1ec>
 804cbaf:	90                   	nop
 804cbb0:	8b 4a 04             	mov    0x4(%edx),%ecx
 804cbb3:	83 e1 fc             	and    $0xfffffffc,%ecx
 804cbb6:	39 c8                	cmp    %ecx,%eax
 804cbb8:	73 07                	jae    804cbc1 <_free_r+0xc1>
 804cbba:	8b 52 08             	mov    0x8(%edx),%edx
 804cbbd:	39 d7                	cmp    %edx,%edi
 804cbbf:	75 ef                	jne    804cbb0 <_free_r+0xb0>
 804cbc1:	8b 42 0c             	mov    0xc(%edx),%eax
 804cbc4:	89 43 0c             	mov    %eax,0xc(%ebx)
 804cbc7:	89 53 08             	mov    %edx,0x8(%ebx)
 804cbca:	89 58 08             	mov    %ebx,0x8(%eax)
 804cbcd:	89 5a 0c             	mov    %ebx,0xc(%edx)

    set_head(p, sz | PREV_INUSE);
    top = p;
    if ((unsigned long)(sz) >= (unsigned long)trim_threshold) 
      malloc_trim(RCALL top_pad); 
    MALLOC_UNLOCK;
 804cbd0:	89 74 24 30          	mov    %esi,0x30(%esp)
    frontlink(p, sz, idx, bck, fwd);  

  MALLOC_UNLOCK;

#endif /* MALLOC_PROVIDED */
}
 804cbd4:	83 c4 1c             	add    $0x1c,%esp
 804cbd7:	5b                   	pop    %ebx
 804cbd8:	5e                   	pop    %esi
 804cbd9:	5f                   	pop    %edi
 804cbda:	5d                   	pop    %ebp

    set_head(p, sz | PREV_INUSE);
    top = p;
    if ((unsigned long)(sz) >= (unsigned long)trim_threshold) 
      malloc_trim(RCALL top_pad); 
    MALLOC_UNLOCK;
 804cbdb:	e9 cc 11 00 00       	jmp    804ddac <__malloc_unlock>

  islr = 0;

  if (!(hd & PREV_INUSE))                    /* consolidate backward */
  {
    prevsz = p->prev_size;
 804cbe0:	8b 7f f8             	mov    -0x8(%edi),%edi
    p = chunk_at_offset(p, -prevsz);
 804cbe3:	29 fb                	sub    %edi,%ebx
    sz += prevsz;
 804cbe5:	01 f8                	add    %edi,%eax
    
    if (p->fd == last_remainder)             /* keep as last_remainder */
 804cbe7:	8b 7b 08             	mov    0x8(%ebx),%edi
 804cbea:	81 ff 68 45 05 08    	cmp    $0x8054568,%edi
 804cbf0:	0f 84 da 00 00 00    	je     804ccd0 <_free_r+0x1d0>
      islr = 1;
    else
      unlink(p, bck, fwd);
 804cbf6:	8b 6b 0c             	mov    0xc(%ebx),%ebp
 804cbf9:	89 6f 0c             	mov    %ebp,0xc(%edi)
 804cbfc:	89 7d 08             	mov    %edi,0x8(%ebp)
    return;
  }

  set_head(next, nextsz);                    /* clear inuse bit */

  islr = 0;
 804cbff:	31 ff                	xor    %edi,%edi
 804cc01:	e9 47 ff ff ff       	jmp    804cb4d <_free_r+0x4d>
 804cc06:	66 90                	xchg   %ax,%ax
    frontlink(p, sz, idx, bck, fwd);  

  MALLOC_UNLOCK;

#endif /* MALLOC_PROVIDED */
}
 804cc08:	83 c4 1c             	add    $0x1c,%esp
 804cc0b:	5b                   	pop    %ebx
 804cc0c:	5e                   	pop    %esi
 804cc0d:	5f                   	pop    %edi
 804cc0e:	5d                   	pop    %ebp
 804cc0f:	c3                   	ret    


  set_head(p, sz | PREV_INUSE);
  set_foot(p, sz);
  if (!islr)
    frontlink(p, sz, idx, bck, fwd);  
 804cc10:	c1 e8 03             	shr    $0x3,%eax
 804cc13:	89 c1                	mov    %eax,%ecx
 804cc15:	c1 f9 02             	sar    $0x2,%ecx
 804cc18:	ba 01 00 00 00       	mov    $0x1,%edx
 804cc1d:	d3 e2                	shl    %cl,%edx
 804cc1f:	09 15 64 45 05 08    	or     %edx,0x8054564
 804cc25:	8d 04 c5 60 45 05 08 	lea    0x8054560(,%eax,8),%eax
 804cc2c:	8b 50 08             	mov    0x8(%eax),%edx
 804cc2f:	89 43 0c             	mov    %eax,0xc(%ebx)
 804cc32:	89 53 08             	mov    %edx,0x8(%ebx)
 804cc35:	89 58 08             	mov    %ebx,0x8(%eax)
 804cc38:	89 5a 0c             	mov    %ebx,0xc(%edx)

    set_head(p, sz | PREV_INUSE);
    top = p;
    if ((unsigned long)(sz) >= (unsigned long)trim_threshold) 
      malloc_trim(RCALL top_pad); 
    MALLOC_UNLOCK;
 804cc3b:	89 74 24 30          	mov    %esi,0x30(%esp)
    frontlink(p, sz, idx, bck, fwd);  

  MALLOC_UNLOCK;

#endif /* MALLOC_PROVIDED */
}
 804cc3f:	83 c4 1c             	add    $0x1c,%esp
 804cc42:	5b                   	pop    %ebx
 804cc43:	5e                   	pop    %esi
 804cc44:	5f                   	pop    %edi
 804cc45:	5d                   	pop    %ebp

    set_head(p, sz | PREV_INUSE);
    top = p;
    if ((unsigned long)(sz) >= (unsigned long)trim_threshold) 
      malloc_trim(RCALL top_pad); 
    MALLOC_UNLOCK;
 804cc46:	e9 61 11 00 00       	jmp    804ddac <__malloc_unlock>
 804cc4b:	90                   	nop
  
  if (!(inuse_bit_at_offset(next, nextsz)))   /* consolidate forward */
  {
    sz += nextsz;
    
    if (!islr && next->fd == last_remainder)  /* re-insert last_remainder */
 804cc4c:	81 f9 68 45 05 08    	cmp    $0x8054568,%ecx
 804cc52:	0f 85 09 ff ff ff    	jne    804cb61 <_free_r+0x61>
    {
      islr = 1;
      link_last_remainder(p);   
 804cc58:	89 1d 74 45 05 08    	mov    %ebx,0x8054574
 804cc5e:	89 1d 70 45 05 08    	mov    %ebx,0x8054570
 804cc64:	c7 43 0c 68 45 05 08 	movl   $0x8054568,0xc(%ebx)
 804cc6b:	c7 43 08 68 45 05 08 	movl   $0x8054568,0x8(%ebx)
    else
      unlink(next, bck, fwd);
  }


  set_head(p, sz | PREV_INUSE);
 804cc72:	89 c2                	mov    %eax,%edx
 804cc74:	83 ca 01             	or     $0x1,%edx
 804cc77:	89 53 04             	mov    %edx,0x4(%ebx)
  set_foot(p, sz);
 804cc7a:	89 04 03             	mov    %eax,(%ebx,%eax,1)
 804cc7d:	e9 4e ff ff ff       	jmp    804cbd0 <_free_r+0xd0>
 804cc82:	66 90                	xchg   %ax,%ax
  next = chunk_at_offset(p, sz);
  nextsz = chunksize(next);
  
  if (next == top)                            /* merge with top */
  {
    sz += nextsz;
 804cc84:	01 c8                	add    %ecx,%eax

    if (!(hd & PREV_INUSE))                    /* consolidate backward */
 804cc86:	83 e5 01             	and    $0x1,%ebp
 804cc89:	75 13                	jne    804cc9e <_free_r+0x19e>
    {
      prevsz = p->prev_size;
 804cc8b:	8b 57 f8             	mov    -0x8(%edi),%edx
      p = chunk_at_offset(p, -prevsz);
 804cc8e:	29 d3                	sub    %edx,%ebx
      sz += prevsz;
 804cc90:	01 d0                	add    %edx,%eax
      unlink(p, bck, fwd);
 804cc92:	8b 53 0c             	mov    0xc(%ebx),%edx
 804cc95:	8b 4b 08             	mov    0x8(%ebx),%ecx
 804cc98:	89 51 0c             	mov    %edx,0xc(%ecx)
 804cc9b:	89 4a 08             	mov    %ecx,0x8(%edx)
    }

    set_head(p, sz | PREV_INUSE);
 804cc9e:	89 c2                	mov    %eax,%edx
 804cca0:	83 ca 01             	or     $0x1,%edx
 804cca3:	89 53 04             	mov    %edx,0x4(%ebx)
    top = p;
 804cca6:	89 1d 68 45 05 08    	mov    %ebx,0x8054568
    if ((unsigned long)(sz) >= (unsigned long)trim_threshold) 
 804ccac:	3b 05 44 45 05 08    	cmp    0x8054544,%eax
 804ccb2:	0f 82 18 ff ff ff    	jb     804cbd0 <_free_r+0xd0>
      malloc_trim(RCALL top_pad); 
 804ccb8:	a1 f0 49 05 08       	mov    0x80549f0,%eax
 804ccbd:	89 44 24 04          	mov    %eax,0x4(%esp)
 804ccc1:	89 34 24             	mov    %esi,(%esp)
 804ccc4:	e8 5f fd ff ff       	call   804ca28 <_malloc_trim_r>
 804ccc9:	e9 02 ff ff ff       	jmp    804cbd0 <_free_r+0xd0>
 804ccce:	66 90                	xchg   %ax,%ax
    prevsz = p->prev_size;
    p = chunk_at_offset(p, -prevsz);
    sz += prevsz;
    
    if (p->fd == last_remainder)             /* keep as last_remainder */
      islr = 1;
 804ccd0:	bf 01 00 00 00       	mov    $0x1,%edi
 804ccd5:	e9 73 fe ff ff       	jmp    804cb4d <_free_r+0x4d>
 804ccda:	66 90                	xchg   %ax,%ax


  set_head(p, sz | PREV_INUSE);
  set_foot(p, sz);
  if (!islr)
    frontlink(p, sz, idx, bck, fwd);  
 804ccdc:	83 f9 14             	cmp    $0x14,%ecx
 804ccdf:	77 23                	ja     804cd04 <_free_r+0x204>
 804cce1:	83 c1 5b             	add    $0x5b,%ecx
 804cce4:	8d 14 09             	lea    (%ecx,%ecx,1),%edx
 804cce7:	e9 b1 fe ff ff       	jmp    804cb9d <_free_r+0x9d>
 804ccec:	c1 f9 02             	sar    $0x2,%ecx
 804ccef:	b8 01 00 00 00       	mov    $0x1,%eax
 804ccf4:	d3 e0                	shl    %cl,%eax
 804ccf6:	09 05 64 45 05 08    	or     %eax,0x8054564
 804ccfc:	89 d0                	mov    %edx,%eax
 804ccfe:	e9 c1 fe ff ff       	jmp    804cbc4 <_free_r+0xc4>
 804cd03:	90                   	nop
 804cd04:	83 f9 54             	cmp    $0x54,%ecx
 804cd07:	77 10                	ja     804cd19 <_free_r+0x219>
 804cd09:	89 c1                	mov    %eax,%ecx
 804cd0b:	c1 e9 0c             	shr    $0xc,%ecx
 804cd0e:	83 c1 6e             	add    $0x6e,%ecx
 804cd11:	8d 14 09             	lea    (%ecx,%ecx,1),%edx
 804cd14:	e9 84 fe ff ff       	jmp    804cb9d <_free_r+0x9d>
 804cd19:	81 f9 54 01 00 00    	cmp    $0x154,%ecx
 804cd1f:	77 10                	ja     804cd31 <_free_r+0x231>
 804cd21:	89 c1                	mov    %eax,%ecx
 804cd23:	c1 e9 0f             	shr    $0xf,%ecx
 804cd26:	83 c1 77             	add    $0x77,%ecx
 804cd29:	8d 14 09             	lea    (%ecx,%ecx,1),%edx
 804cd2c:	e9 6c fe ff ff       	jmp    804cb9d <_free_r+0x9d>
 804cd31:	81 f9 54 05 00 00    	cmp    $0x554,%ecx
 804cd37:	77 10                	ja     804cd49 <_free_r+0x249>
 804cd39:	89 c1                	mov    %eax,%ecx
 804cd3b:	c1 e9 12             	shr    $0x12,%ecx
 804cd3e:	83 c1 7c             	add    $0x7c,%ecx
 804cd41:	8d 14 09             	lea    (%ecx,%ecx,1),%edx
 804cd44:	e9 54 fe ff ff       	jmp    804cb9d <_free_r+0x9d>
 804cd49:	ba fc 00 00 00       	mov    $0xfc,%edx
 804cd4e:	b9 7e 00 00 00       	mov    $0x7e,%ecx
 804cd53:	e9 45 fe ff ff       	jmp    804cb9d <_free_r+0x9d>

0804cd58 <__sfvwrite_r>:
int
_DEFUN(__sfvwrite_r, (ptr, fp, uio),
       struct _reent *ptr _AND
       register FILE *fp _AND
       register struct __suio *uio)
{
 804cd58:	55                   	push   %ebp
 804cd59:	57                   	push   %edi
 804cd5a:	56                   	push   %esi
 804cd5b:	53                   	push   %ebx
 804cd5c:	83 ec 3c             	sub    $0x3c,%esp
 804cd5f:	8b 5c 24 54          	mov    0x54(%esp),%ebx
  register struct __siov *iov;
  register _READ_WRITE_RETURN_TYPE w, s;
  char *nl;
  int nlknown, nldist;

  if ((len = uio->uio_resid) == 0)
 804cd63:	8b 4c 24 58          	mov    0x58(%esp),%ecx
 804cd67:	8b 49 08             	mov    0x8(%ecx),%ecx
 804cd6a:	85 c9                	test   %ecx,%ecx
 804cd6c:	74 63                	je     804cdd1 <__sfvwrite_r+0x79>
    return 0;

  /* make sure we can write */
  if (cantwrite (ptr, fp))
 804cd6e:	8b 43 0c             	mov    0xc(%ebx),%eax
 804cd71:	a8 08                	test   $0x8,%al
 804cd73:	74 67                	je     804cddc <__sfvwrite_r+0x84>
 804cd75:	8b 53 10             	mov    0x10(%ebx),%edx
 804cd78:	85 d2                	test   %edx,%edx
 804cd7a:	74 60                	je     804cddc <__sfvwrite_r+0x84>
    return EOF;

  iov = uio->uio_iov;
 804cd7c:	8b 4c 24 58          	mov    0x58(%esp),%ecx
 804cd80:	8b 29                	mov    (%ecx),%ebp
      while (uio->uio_resid > 0);
      return 0;
    }
#endif

  if (fp->_flags & __SNBF)
 804cd82:	a8 02                	test   $0x2,%al
 804cd84:	74 7b                	je     804ce01 <__sfvwrite_r+0xa9>
 804cd86:	31 ff                	xor    %edi,%edi
 804cd88:	31 f6                	xor    %esi,%esi
 804cd8a:	66 90                	xchg   %ax,%ax
       * Unbuffered: Split buffer in the largest multiple of BUFSIZ < INT_MAX
       * as some legacy code may expect int instead of size_t.
       */
      do
	{
	  GETIOV (;);
 804cd8c:	85 f6                	test   %esi,%esi
 804cd8e:	0f 84 24 01 00 00    	je     804ceb8 <__sfvwrite_r+0x160>
	  w = fp->_write (ptr, fp->_cookie, p,
			  MIN (len, INT_MAX - INT_MAX % BUFSIZ));
 804cd94:	89 f0                	mov    %esi,%eax
 804cd96:	81 fe 00 fc ff 7f    	cmp    $0x7ffffc00,%esi
 804cd9c:	76 05                	jbe    804cda3 <__sfvwrite_r+0x4b>
 804cd9e:	b8 00 fc ff 7f       	mov    $0x7ffffc00,%eax
       * as some legacy code may expect int instead of size_t.
       */
      do
	{
	  GETIOV (;);
	  w = fp->_write (ptr, fp->_cookie, p,
 804cda3:	89 44 24 0c          	mov    %eax,0xc(%esp)
 804cda7:	89 7c 24 08          	mov    %edi,0x8(%esp)
 804cdab:	8b 43 1c             	mov    0x1c(%ebx),%eax
 804cdae:	89 44 24 04          	mov    %eax,0x4(%esp)
 804cdb2:	8b 4c 24 50          	mov    0x50(%esp),%ecx
 804cdb6:	89 0c 24             	mov    %ecx,(%esp)
 804cdb9:	ff 53 24             	call   *0x24(%ebx)
			  MIN (len, INT_MAX - INT_MAX % BUFSIZ));
	  if (w <= 0)
 804cdbc:	85 c0                	test   %eax,%eax
 804cdbe:	0f 8e 30 01 00 00    	jle    804cef4 <__sfvwrite_r+0x19c>
	    goto err;
	  p += w;
 804cdc4:	01 c7                	add    %eax,%edi
	  len -= w;
 804cdc6:	29 c6                	sub    %eax,%esi
	}
      while ((uio->uio_resid -= w) != 0);
 804cdc8:	8b 4c 24 58          	mov    0x58(%esp),%ecx
 804cdcc:	29 41 08             	sub    %eax,0x8(%ecx)
 804cdcf:	75 bb                	jne    804cd8c <__sfvwrite_r+0x34>
  register _READ_WRITE_RETURN_TYPE w, s;
  char *nl;
  int nlknown, nldist;

  if ((len = uio->uio_resid) == 0)
    return 0;
 804cdd1:	31 c0                	xor    %eax,%eax
  return 0;

err:
  fp->_flags |= __SERR;
  return EOF;
}
 804cdd3:	83 c4 3c             	add    $0x3c,%esp
 804cdd6:	5b                   	pop    %ebx
 804cdd7:	5e                   	pop    %esi
 804cdd8:	5f                   	pop    %edi
 804cdd9:	5d                   	pop    %ebp
 804cdda:	c3                   	ret    
 804cddb:	90                   	nop

  if ((len = uio->uio_resid) == 0)
    return 0;

  /* make sure we can write */
  if (cantwrite (ptr, fp))
 804cddc:	89 5c 24 04          	mov    %ebx,0x4(%esp)
 804cde0:	8b 4c 24 50          	mov    0x50(%esp),%ecx
 804cde4:	89 0c 24             	mov    %ecx,(%esp)
 804cde7:	e8 cc df ff ff       	call   804adb8 <__swsetup_r>
 804cdec:	85 c0                	test   %eax,%eax
 804cdee:	0f 85 dc 03 00 00    	jne    804d1d0 <__sfvwrite_r+0x478>
 804cdf4:	8b 43 0c             	mov    0xc(%ebx),%eax
    return EOF;

  iov = uio->uio_iov;
 804cdf7:	8b 4c 24 58          	mov    0x58(%esp),%ecx
 804cdfb:	8b 29                	mov    (%ecx),%ebp
      while (uio->uio_resid > 0);
      return 0;
    }
#endif

  if (fp->_flags & __SNBF)
 804cdfd:	a8 02                	test   $0x2,%al
 804cdff:	75 85                	jne    804cd86 <__sfvwrite_r+0x2e>
	  p += w;
	  len -= w;
	}
      while ((uio->uio_resid -= w) != 0);
    }
  else if ((fp->_flags & __SLBF) == 0)
 804ce01:	c7 44 24 24 00 00 00 	movl   $0x0,0x24(%esp)
 804ce08:	00 
 804ce09:	a8 01                	test   $0x1,%al
 804ce0b:	0f 84 fb 00 00 00    	je     804cf0c <__sfvwrite_r+0x1b4>
 804ce11:	c7 44 24 2c 00 00 00 	movl   $0x0,0x2c(%esp)
 804ce18:	00 
 804ce19:	c7 44 24 28 00 00 00 	movl   $0x0,0x28(%esp)
 804ce20:	00 
 804ce21:	31 ff                	xor    %edi,%edi
 804ce23:	90                   	nop
       */
      nlknown = 0;
      nldist = 0;
      do
	{
	  GETIOV (nlknown = 0);
 804ce24:	85 ff                	test   %edi,%edi
 804ce26:	74 75                	je     804ce9d <__sfvwrite_r+0x145>
	  if (!nlknown)
 804ce28:	8b 44 24 2c          	mov    0x2c(%esp),%eax
 804ce2c:	85 c0                	test   %eax,%eax
 804ce2e:	0f 84 1c 02 00 00    	je     804d050 <__sfvwrite_r+0x2f8>
	    {
	      nl = memchr ((_PTR) p, '\n', len);
	      nldist = nl ? nl + 1 - p : len + 1;
	      nlknown = 1;
	    }
	  s = MIN (len, nldist);
 804ce34:	8b 54 24 24          	mov    0x24(%esp),%edx
 804ce38:	39 fa                	cmp    %edi,%edx
 804ce3a:	76 02                	jbe    804ce3e <__sfvwrite_r+0xe6>
 804ce3c:	89 fa                	mov    %edi,%edx
	  w = fp->_w + fp->_bf._size;
 804ce3e:	8b 73 08             	mov    0x8(%ebx),%esi
 804ce41:	8b 43 14             	mov    0x14(%ebx),%eax
	  if (fp->_p > fp->_bf._base && s > w)
 804ce44:	8b 0b                	mov    (%ebx),%ecx
 804ce46:	3b 4b 10             	cmp    0x10(%ebx),%ecx
 804ce49:	76 06                	jbe    804ce51 <__sfvwrite_r+0xf9>
	      nl = memchr ((_PTR) p, '\n', len);
	      nldist = nl ? nl + 1 - p : len + 1;
	      nlknown = 1;
	    }
	  s = MIN (len, nldist);
	  w = fp->_w + fp->_bf._size;
 804ce4b:	01 c6                	add    %eax,%esi
	  if (fp->_p > fp->_bf._base && s > w)
 804ce4d:	39 f2                	cmp    %esi,%edx
 804ce4f:	7f 77                	jg     804cec8 <__sfvwrite_r+0x170>
	      /* fp->_w -= w; */
	      fp->_p += w;
	      if (_fflush_r (ptr, fp))
		goto err;
	    }
	  else if (s >= (w = fp->_bf._size))
 804ce51:	39 c2                	cmp    %eax,%edx
 804ce53:	0f 8c 8f 01 00 00    	jl     804cfe8 <__sfvwrite_r+0x290>
	    {
	      w = fp->_write (ptr, fp->_cookie, p, w);
 804ce59:	89 44 24 0c          	mov    %eax,0xc(%esp)
 804ce5d:	8b 54 24 28          	mov    0x28(%esp),%edx
 804ce61:	89 54 24 08          	mov    %edx,0x8(%esp)
 804ce65:	8b 43 1c             	mov    0x1c(%ebx),%eax
 804ce68:	89 44 24 04          	mov    %eax,0x4(%esp)
 804ce6c:	8b 4c 24 50          	mov    0x50(%esp),%ecx
 804ce70:	89 0c 24             	mov    %ecx,(%esp)
 804ce73:	ff 53 24             	call   *0x24(%ebx)
 804ce76:	89 c6                	mov    %eax,%esi
	      if (w <= 0)
 804ce78:	85 c0                	test   %eax,%eax
 804ce7a:	7e 78                	jle    804cef4 <__sfvwrite_r+0x19c>
	      w = s;
	      COPY (w);
	      fp->_w -= w;
	      fp->_p += w;
	    }
	  if ((nldist -= w) == 0)
 804ce7c:	29 74 24 24          	sub    %esi,0x24(%esp)
 804ce80:	0f 84 8f 01 00 00    	je     804d015 <__sfvwrite_r+0x2bd>
	      /* copied the newline: flush and forget */
	      if (_fflush_r (ptr, fp))
		goto err;
	      nlknown = 0;
	    }
	  p += w;
 804ce86:	01 74 24 28          	add    %esi,0x28(%esp)
	  len -= w;
 804ce8a:	29 f7                	sub    %esi,%edi
	}
      while ((uio->uio_resid -= w) != 0);
 804ce8c:	8b 4c 24 58          	mov    0x58(%esp),%ecx
 804ce90:	29 71 08             	sub    %esi,0x8(%ecx)
 804ce93:	0f 84 38 ff ff ff    	je     804cdd1 <__sfvwrite_r+0x79>
       */
      nlknown = 0;
      nldist = 0;
      do
	{
	  GETIOV (nlknown = 0);
 804ce99:	85 ff                	test   %edi,%edi
 804ce9b:	75 8b                	jne    804ce28 <__sfvwrite_r+0xd0>
 804ce9d:	8b 55 00             	mov    0x0(%ebp),%edx
 804cea0:	89 54 24 28          	mov    %edx,0x28(%esp)
 804cea4:	8b 7d 04             	mov    0x4(%ebp),%edi
 804cea7:	83 c5 08             	add    $0x8,%ebp
 804ceaa:	c7 44 24 2c 00 00 00 	movl   $0x0,0x2c(%esp)
 804ceb1:	00 
 804ceb2:	e9 6d ff ff ff       	jmp    804ce24 <__sfvwrite_r+0xcc>
 804ceb7:	90                   	nop
       * Unbuffered: Split buffer in the largest multiple of BUFSIZ < INT_MAX
       * as some legacy code may expect int instead of size_t.
       */
      do
	{
	  GETIOV (;);
 804ceb8:	8b 7d 00             	mov    0x0(%ebp),%edi
 804cebb:	8b 75 04             	mov    0x4(%ebp),%esi
 804cebe:	83 c5 08             	add    $0x8,%ebp
 804cec1:	e9 c6 fe ff ff       	jmp    804cd8c <__sfvwrite_r+0x34>
 804cec6:	66 90                	xchg   %ax,%ax
	    }
	  s = MIN (len, nldist);
	  w = fp->_w + fp->_bf._size;
	  if (fp->_p > fp->_bf._base && s > w)
	    {
	      COPY (w);
 804cec8:	89 74 24 08          	mov    %esi,0x8(%esp)
 804cecc:	8b 54 24 28          	mov    0x28(%esp),%edx
 804ced0:	89 54 24 04          	mov    %edx,0x4(%esp)
 804ced4:	89 0c 24             	mov    %ecx,(%esp)
 804ced7:	e8 b4 0d 00 00       	call   804dc90 <memmove>
	      /* fp->_w -= w; */
	      fp->_p += w;
 804cedc:	01 33                	add    %esi,(%ebx)
	      if (_fflush_r (ptr, fp))
 804cede:	89 5c 24 04          	mov    %ebx,0x4(%esp)
 804cee2:	8b 4c 24 50          	mov    0x50(%esp),%ecx
 804cee6:	89 0c 24             	mov    %ecx,(%esp)
 804cee9:	e8 d2 f7 ff ff       	call   804c6c0 <_fflush_r>
 804ceee:	85 c0                	test   %eax,%eax
 804cef0:	74 8a                	je     804ce7c <__sfvwrite_r+0x124>
 804cef2:	66 90                	xchg   %ax,%ax
 804cef4:	8b 43 0c             	mov    0xc(%ebx),%eax
      while ((uio->uio_resid -= w) != 0);
    }
  return 0;

err:
  fp->_flags |= __SERR;
 804cef7:	83 c8 40             	or     $0x40,%eax
 804cefa:	66 89 43 0c          	mov    %ax,0xc(%ebx)
  return EOF;
 804cefe:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
}
 804cf03:	83 c4 3c             	add    $0x3c,%esp
 804cf06:	5b                   	pop    %ebx
 804cf07:	5e                   	pop    %esi
 804cf08:	5f                   	pop    %edi
 804cf09:	5d                   	pop    %ebp
 804cf0a:	c3                   	ret    
 804cf0b:	90                   	nop
	  p += w;
	  len -= w;
	}
      while ((uio->uio_resid -= w) != 0);
    }
  else if ((fp->_flags & __SLBF) == 0)
 804cf0c:	31 f6                	xor    %esi,%esi
 804cf0e:	66 90                	xchg   %ax,%ax
       * we are dealing with the asprintf routines, we will
       * dynamically increase the buffer size as needed.
       */
      do
	{
	  GETIOV (;);
 804cf10:	85 f6                	test   %esi,%esi
 804cf12:	74 6c                	je     804cf80 <__sfvwrite_r+0x228>
	  w = fp->_w;
 804cf14:	8b 53 08             	mov    0x8(%ebx),%edx
	  if (fp->_flags & __SSTR)
 804cf17:	f6 c4 02             	test   $0x2,%ah
 804cf1a:	74 74                	je     804cf90 <__sfvwrite_r+0x238>
	    {
	      if (len >= w && fp->_flags & (__SMBF | __SOPT))
 804cf1c:	89 d1                	mov    %edx,%ecx
 804cf1e:	39 d6                	cmp    %edx,%esi
 804cf20:	0f 82 16 01 00 00    	jb     804d03c <__sfvwrite_r+0x2e4>
 804cf26:	a9 80 04 00 00       	test   $0x480,%eax
 804cf2b:	0f 85 57 01 00 00    	jne    804d088 <__sfvwrite_r+0x330>
 804cf31:	8b 03                	mov    (%ebx),%eax
 804cf33:	89 44 24 2c          	mov    %eax,0x2c(%esp)
 804cf37:	89 f7                	mov    %esi,%edi
		  w = len;
		  fp->_w = newsize - curpos;
		}
	      if (len < w)
		w = len;
	      COPY (w);		/* copy MIN(fp->_w,len), */
 804cf39:	89 4c 24 08          	mov    %ecx,0x8(%esp)
 804cf3d:	8b 44 24 24          	mov    0x24(%esp),%eax
 804cf41:	89 44 24 04          	mov    %eax,0x4(%esp)
 804cf45:	8b 44 24 2c          	mov    0x2c(%esp),%eax
 804cf49:	89 04 24             	mov    %eax,(%esp)
 804cf4c:	89 54 24 1c          	mov    %edx,0x1c(%esp)
 804cf50:	89 4c 24 20          	mov    %ecx,0x20(%esp)
 804cf54:	e8 37 0d 00 00       	call   804dc90 <memmove>
	      fp->_w -= w;
 804cf59:	8b 54 24 1c          	mov    0x1c(%esp),%edx
 804cf5d:	29 53 08             	sub    %edx,0x8(%ebx)
	      fp->_p += w;
 804cf60:	8b 4c 24 20          	mov    0x20(%esp),%ecx
 804cf64:	01 0b                	add    %ecx,(%ebx)
	      w = ((int)MIN (len, INT_MAX)) / fp->_bf._size * fp->_bf._size;
	      w = fp->_write (ptr, fp->_cookie, p, w);
	      if (w <= 0)
		goto err;
	    }
	  p += w;
 804cf66:	01 7c 24 24          	add    %edi,0x24(%esp)
	  len -= w;
 804cf6a:	29 fe                	sub    %edi,%esi
	}
      while ((uio->uio_resid -= w) != 0);
 804cf6c:	8b 4c 24 58          	mov    0x58(%esp),%ecx
 804cf70:	29 79 08             	sub    %edi,0x8(%ecx)
 804cf73:	0f 84 58 fe ff ff    	je     804cdd1 <__sfvwrite_r+0x79>
 804cf79:	8b 43 0c             	mov    0xc(%ebx),%eax
       * we are dealing with the asprintf routines, we will
       * dynamically increase the buffer size as needed.
       */
      do
	{
	  GETIOV (;);
 804cf7c:	85 f6                	test   %esi,%esi
 804cf7e:	75 94                	jne    804cf14 <__sfvwrite_r+0x1bc>
 804cf80:	8b 4d 00             	mov    0x0(%ebp),%ecx
 804cf83:	89 4c 24 24          	mov    %ecx,0x24(%esp)
 804cf87:	8b 75 04             	mov    0x4(%ebp),%esi
 804cf8a:	83 c5 08             	add    $0x8,%ebp
 804cf8d:	eb 81                	jmp    804cf10 <__sfvwrite_r+0x1b8>
 804cf8f:	90                   	nop
	      COPY (w);		/* copy MIN(fp->_w,len), */
	      fp->_w -= w;
	      fp->_p += w;
	      w = len;		/* but pretend copied all */
	    }
	  else if (fp->_p > fp->_bf._base || len < fp->_bf._size)
 804cf90:	8b 03                	mov    (%ebx),%eax
 804cf92:	3b 43 10             	cmp    0x10(%ebx),%eax
 804cf95:	77 0b                	ja     804cfa2 <__sfvwrite_r+0x24a>
 804cf97:	8b 4b 14             	mov    0x14(%ebx),%ecx
 804cf9a:	39 ce                	cmp    %ecx,%esi
 804cf9c:	0f 83 8a 01 00 00    	jae    804d12c <__sfvwrite_r+0x3d4>
	    {
	      /* pass through the buffer */
	      w = MIN (len, w);
 804cfa2:	89 f7                	mov    %esi,%edi
 804cfa4:	39 d6                	cmp    %edx,%esi
 804cfa6:	76 02                	jbe    804cfaa <__sfvwrite_r+0x252>
 804cfa8:	89 d7                	mov    %edx,%edi
	      COPY (w);
 804cfaa:	89 7c 24 08          	mov    %edi,0x8(%esp)
 804cfae:	8b 4c 24 24          	mov    0x24(%esp),%ecx
 804cfb2:	89 4c 24 04          	mov    %ecx,0x4(%esp)
 804cfb6:	89 04 24             	mov    %eax,(%esp)
 804cfb9:	e8 d2 0c 00 00       	call   804dc90 <memmove>
	      fp->_w -= w;
 804cfbe:	8b 43 08             	mov    0x8(%ebx),%eax
 804cfc1:	29 f8                	sub    %edi,%eax
 804cfc3:	89 43 08             	mov    %eax,0x8(%ebx)
	      fp->_p += w;
 804cfc6:	01 3b                	add    %edi,(%ebx)
	      if (fp->_w == 0 && _fflush_r (ptr, fp))
 804cfc8:	85 c0                	test   %eax,%eax
 804cfca:	75 9a                	jne    804cf66 <__sfvwrite_r+0x20e>
 804cfcc:	89 5c 24 04          	mov    %ebx,0x4(%esp)
 804cfd0:	8b 4c 24 50          	mov    0x50(%esp),%ecx
 804cfd4:	89 0c 24             	mov    %ecx,(%esp)
 804cfd7:	e8 e4 f6 ff ff       	call   804c6c0 <_fflush_r>
 804cfdc:	85 c0                	test   %eax,%eax
 804cfde:	74 86                	je     804cf66 <__sfvwrite_r+0x20e>
 804cfe0:	e9 0f ff ff ff       	jmp    804cef4 <__sfvwrite_r+0x19c>
 804cfe5:	8d 76 00             	lea    0x0(%esi),%esi
		goto err;
	    }
	  else
	    {
	      w = s;
	      COPY (w);
 804cfe8:	89 54 24 08          	mov    %edx,0x8(%esp)
 804cfec:	8b 44 24 28          	mov    0x28(%esp),%eax
 804cff0:	89 44 24 04          	mov    %eax,0x4(%esp)
 804cff4:	89 0c 24             	mov    %ecx,(%esp)
 804cff7:	89 54 24 1c          	mov    %edx,0x1c(%esp)
 804cffb:	e8 90 0c 00 00       	call   804dc90 <memmove>
	      fp->_w -= w;
 804d000:	8b 54 24 1c          	mov    0x1c(%esp),%edx
 804d004:	29 53 08             	sub    %edx,0x8(%ebx)
	      fp->_p += w;
 804d007:	01 13                	add    %edx,(%ebx)
 804d009:	89 d6                	mov    %edx,%esi
	    }
	  if ((nldist -= w) == 0)
 804d00b:	29 74 24 24          	sub    %esi,0x24(%esp)
 804d00f:	0f 85 71 fe ff ff    	jne    804ce86 <__sfvwrite_r+0x12e>
	    {
	      /* copied the newline: flush and forget */
	      if (_fflush_r (ptr, fp))
 804d015:	89 5c 24 04          	mov    %ebx,0x4(%esp)
 804d019:	8b 4c 24 50          	mov    0x50(%esp),%ecx
 804d01d:	89 0c 24             	mov    %ecx,(%esp)
 804d020:	e8 9b f6 ff ff       	call   804c6c0 <_fflush_r>
 804d025:	85 c0                	test   %eax,%eax
 804d027:	0f 85 c7 fe ff ff    	jne    804cef4 <__sfvwrite_r+0x19c>
		goto err;
	      nlknown = 0;
 804d02d:	c7 44 24 2c 00 00 00 	movl   $0x0,0x2c(%esp)
 804d034:	00 
 804d035:	e9 4c fe ff ff       	jmp    804ce86 <__sfvwrite_r+0x12e>
 804d03a:	66 90                	xchg   %ax,%ax
		  fp->_bf._size = newsize;
		  w = len;
		  fp->_w = newsize - curpos;
		}
	      if (len < w)
		w = len;
 804d03c:	89 f2                	mov    %esi,%edx
 804d03e:	8b 03                	mov    (%ebx),%eax
 804d040:	89 44 24 2c          	mov    %eax,0x2c(%esp)
 804d044:	89 f7                	mov    %esi,%edi
 804d046:	89 f1                	mov    %esi,%ecx
 804d048:	e9 ec fe ff ff       	jmp    804cf39 <__sfvwrite_r+0x1e1>
 804d04d:	8d 76 00             	lea    0x0(%esi),%esi
      do
	{
	  GETIOV (nlknown = 0);
	  if (!nlknown)
	    {
	      nl = memchr ((_PTR) p, '\n', len);
 804d050:	89 7c 24 08          	mov    %edi,0x8(%esp)
 804d054:	c7 44 24 04 0a 00 00 	movl   $0xa,0x4(%esp)
 804d05b:	00 
 804d05c:	8b 4c 24 28          	mov    0x28(%esp),%ecx
 804d060:	89 0c 24             	mov    %ecx,(%esp)
 804d063:	e8 48 0b 00 00       	call   804dbb0 <memchr>
	      nldist = nl ? nl + 1 - p : len + 1;
 804d068:	85 c0                	test   %eax,%eax
 804d06a:	0f 84 4c 01 00 00    	je     804d1bc <__sfvwrite_r+0x464>
 804d070:	40                   	inc    %eax
 804d071:	8b 54 24 28          	mov    0x28(%esp),%edx
 804d075:	29 d0                	sub    %edx,%eax
 804d077:	89 44 24 24          	mov    %eax,0x24(%esp)
	      nlknown = 1;
 804d07b:	c7 44 24 2c 01 00 00 	movl   $0x1,0x2c(%esp)
 804d082:	00 
 804d083:	e9 ac fd ff ff       	jmp    804ce34 <__sfvwrite_r+0xdc>
	  if (fp->_flags & __SSTR)
	    {
	      if (len >= w && fp->_flags & (__SMBF | __SOPT))
		{ /* must be asprintf family */
		  unsigned char *str;
		  int curpos = (fp->_p - fp->_bf._base);
 804d088:	8b 53 10             	mov    0x10(%ebx),%edx
 804d08b:	89 54 24 18          	mov    %edx,0x18(%esp)
 804d08f:	8b 0b                	mov    (%ebx),%ecx
 804d091:	29 d1                	sub    %edx,%ecx
 804d093:	89 4c 24 28          	mov    %ecx,0x28(%esp)
		     than (1+sqrt(5))/2 to accomodate malloc
		     overhead. asprintf EXPECTS us to overallocate, so
		     that it can add a trailing \0 without
		     reallocating.  The new allocation should thus be
		     max(prev_size*1.5, curpos+len+1). */
		  int newsize = fp->_bf._size * 3 / 2;
 804d097:	8b 53 14             	mov    0x14(%ebx),%edx
 804d09a:	8d 14 52             	lea    (%edx,%edx,2),%edx
 804d09d:	89 d7                	mov    %edx,%edi
 804d09f:	c1 ef 1f             	shr    $0x1f,%edi
 804d0a2:	01 d7                	add    %edx,%edi
 804d0a4:	d1 ff                	sar    %edi
		  if (newsize < curpos + len + 1)
 804d0a6:	89 fa                	mov    %edi,%edx
 804d0a8:	8d 4c 0e 01          	lea    0x1(%esi,%ecx,1),%ecx
 804d0ac:	89 4c 24 2c          	mov    %ecx,0x2c(%esp)
 804d0b0:	39 cf                	cmp    %ecx,%edi
 804d0b2:	73 04                	jae    804d0b8 <__sfvwrite_r+0x360>
		    newsize = curpos + len + 1;
 804d0b4:	89 cf                	mov    %ecx,%edi
 804d0b6:	89 ca                	mov    %ecx,%edx
		  if (fp->_flags & __SOPT)
 804d0b8:	f6 c4 04             	test   $0x4,%ah
 804d0bb:	0f 84 af 00 00 00    	je     804d170 <__sfvwrite_r+0x418>
		    {
		      /* asnprintf leaves original buffer alone.  */
		      str = (unsigned char *)_malloc_r (ptr, newsize);
 804d0c1:	89 54 24 04          	mov    %edx,0x4(%esp)
 804d0c5:	8b 4c 24 50          	mov    0x50(%esp),%ecx
 804d0c9:	89 0c 24             	mov    %ecx,(%esp)
 804d0cc:	e8 eb 03 00 00       	call   804d4bc <_malloc_r>
 804d0d1:	89 c2                	mov    %eax,%edx
		      if (!str)
 804d0d3:	85 c0                	test   %eax,%eax
 804d0d5:	0f 84 ff 00 00 00    	je     804d1da <__sfvwrite_r+0x482>
			{
			  ptr->_errno = ENOMEM;
			  goto err;
			}
		      memcpy (str, fp->_bf._base, curpos);
 804d0db:	8b 4c 24 28          	mov    0x28(%esp),%ecx
 804d0df:	89 4c 24 08          	mov    %ecx,0x8(%esp)
 804d0e3:	8b 43 10             	mov    0x10(%ebx),%eax
 804d0e6:	89 44 24 04          	mov    %eax,0x4(%esp)
 804d0ea:	89 14 24             	mov    %edx,(%esp)
 804d0ed:	89 54 24 1c          	mov    %edx,0x1c(%esp)
 804d0f1:	e8 52 0b 00 00       	call   804dc48 <memcpy>
		      fp->_flags = (fp->_flags & ~__SOPT) | __SMBF;
 804d0f6:	8b 43 0c             	mov    0xc(%ebx),%eax
 804d0f9:	25 7f fb ff ff       	and    $0xfffffb7f,%eax
 804d0fe:	0c 80                	or     $0x80,%al
 804d100:	66 89 43 0c          	mov    %ax,0xc(%ebx)
 804d104:	8b 54 24 1c          	mov    0x1c(%esp),%edx
			  /* Ensure correct errno, even if free changed it.  */
			  ptr->_errno = ENOMEM;
			  goto err;
			}
		    }
		  fp->_bf._base = str;
 804d108:	89 53 10             	mov    %edx,0x10(%ebx)
		  fp->_p = str + curpos;
 804d10b:	03 54 24 28          	add    0x28(%esp),%edx
 804d10f:	89 54 24 2c          	mov    %edx,0x2c(%esp)
 804d113:	89 13                	mov    %edx,(%ebx)
		  fp->_bf._size = newsize;
 804d115:	89 7b 14             	mov    %edi,0x14(%ebx)
		  w = len;
 804d118:	89 f2                	mov    %esi,%edx
		  fp->_w = newsize - curpos;
 804d11a:	2b 7c 24 28          	sub    0x28(%esp),%edi
 804d11e:	89 7b 08             	mov    %edi,0x8(%ebx)
 804d121:	89 f7                	mov    %esi,%edi
 804d123:	89 f1                	mov    %esi,%ecx
 804d125:	e9 0f fe ff ff       	jmp    804cf39 <__sfvwrite_r+0x1e1>
 804d12a:	66 90                	xchg   %ax,%ax
		goto err;
	    }
	  else
	    {
	      /* write directly */
	      w = ((int)MIN (len, INT_MAX)) / fp->_bf._size * fp->_bf._size;
 804d12c:	89 f0                	mov    %esi,%eax
 804d12e:	81 fe ff ff ff 7f    	cmp    $0x7fffffff,%esi
 804d134:	76 05                	jbe    804d13b <__sfvwrite_r+0x3e3>
 804d136:	b8 ff ff ff 7f       	mov    $0x7fffffff,%eax
 804d13b:	99                   	cltd   
 804d13c:	f7 f9                	idiv   %ecx
 804d13e:	0f af c1             	imul   %ecx,%eax
	      w = fp->_write (ptr, fp->_cookie, p, w);
 804d141:	89 44 24 0c          	mov    %eax,0xc(%esp)
 804d145:	8b 4c 24 24          	mov    0x24(%esp),%ecx
 804d149:	89 4c 24 08          	mov    %ecx,0x8(%esp)
 804d14d:	8b 43 1c             	mov    0x1c(%ebx),%eax
 804d150:	89 44 24 04          	mov    %eax,0x4(%esp)
 804d154:	8b 4c 24 50          	mov    0x50(%esp),%ecx
 804d158:	89 0c 24             	mov    %ecx,(%esp)
 804d15b:	ff 53 24             	call   *0x24(%ebx)
	      if (w <= 0)
 804d15e:	85 c0                	test   %eax,%eax
 804d160:	0f 8e 8e fd ff ff    	jle    804cef4 <__sfvwrite_r+0x19c>
 804d166:	89 c7                	mov    %eax,%edi
 804d168:	e9 f9 fd ff ff       	jmp    804cf66 <__sfvwrite_r+0x20e>
 804d16d:	8d 76 00             	lea    0x0(%esi),%esi
		      memcpy (str, fp->_bf._base, curpos);
		      fp->_flags = (fp->_flags & ~__SOPT) | __SMBF;
		    }
		  else
		    {
		      str = (unsigned char *)_realloc_r (ptr, fp->_bf._base,
 804d170:	89 54 24 08          	mov    %edx,0x8(%esp)
 804d174:	8b 44 24 18          	mov    0x18(%esp),%eax
 804d178:	89 44 24 04          	mov    %eax,0x4(%esp)
 804d17c:	8b 4c 24 50          	mov    0x50(%esp),%ecx
 804d180:	89 0c 24             	mov    %ecx,(%esp)
 804d183:	e8 ac 19 00 00       	call   804eb34 <_realloc_r>
 804d188:	89 c2                	mov    %eax,%edx
							 newsize);
		      if (!str)
 804d18a:	85 c0                	test   %eax,%eax
 804d18c:	0f 85 76 ff ff ff    	jne    804d108 <__sfvwrite_r+0x3b0>
			{
			  /* Free buffer which is no longer used and clear
			     __SMBF flag to avoid double free in fclose.  */
			  _free_r (ptr, fp->_bf._base);
 804d192:	8b 43 10             	mov    0x10(%ebx),%eax
 804d195:	89 44 24 04          	mov    %eax,0x4(%esp)
 804d199:	8b 4c 24 50          	mov    0x50(%esp),%ecx
 804d19d:	89 0c 24             	mov    %ecx,(%esp)
 804d1a0:	e8 5b f9 ff ff       	call   804cb00 <_free_r>
			  fp->_flags &=  ~__SMBF;
 804d1a5:	8b 43 0c             	mov    0xc(%ebx),%eax
 804d1a8:	24 7f                	and    $0x7f,%al
			  /* Ensure correct errno, even if free changed it.  */
			  ptr->_errno = ENOMEM;
 804d1aa:	8b 4c 24 50          	mov    0x50(%esp),%ecx
 804d1ae:	c7 01 0c 00 00 00    	movl   $0xc,(%ecx)
			  goto err;
 804d1b4:	e9 3e fd ff ff       	jmp    804cef7 <__sfvwrite_r+0x19f>
 804d1b9:	8d 76 00             	lea    0x0(%esi),%esi
	{
	  GETIOV (nlknown = 0);
	  if (!nlknown)
	    {
	      nl = memchr ((_PTR) p, '\n', len);
	      nldist = nl ? nl + 1 - p : len + 1;
 804d1bc:	8d 4f 01             	lea    0x1(%edi),%ecx
 804d1bf:	89 4c 24 24          	mov    %ecx,0x24(%esp)
	      nlknown = 1;
 804d1c3:	c7 44 24 2c 01 00 00 	movl   $0x1,0x2c(%esp)
 804d1ca:	00 
 804d1cb:	e9 64 fc ff ff       	jmp    804ce34 <__sfvwrite_r+0xdc>
  if ((len = uio->uio_resid) == 0)
    return 0;

  /* make sure we can write */
  if (cantwrite (ptr, fp))
    return EOF;
 804d1d0:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
 804d1d5:	e9 f9 fb ff ff       	jmp    804cdd3 <__sfvwrite_r+0x7b>
		    {
		      /* asnprintf leaves original buffer alone.  */
		      str = (unsigned char *)_malloc_r (ptr, newsize);
		      if (!str)
			{
			  ptr->_errno = ENOMEM;
 804d1da:	8b 4c 24 50          	mov    0x50(%esp),%ecx
 804d1de:	c7 01 0c 00 00 00    	movl   $0xc,(%ecx)
 804d1e4:	8b 43 0c             	mov    0xc(%ebx),%eax
			  goto err;
 804d1e7:	e9 0b fd ff ff       	jmp    804cef7 <__sfvwrite_r+0x19f>

0804d1ec <_fwalk>:

int
_DEFUN(_fwalk, (ptr, function),
       struct _reent *ptr _AND
       register int (*function) (FILE *))
{
 804d1ec:	55                   	push   %ebp
 804d1ed:	57                   	push   %edi
 804d1ee:	56                   	push   %esi
 804d1ef:	53                   	push   %ebx
 804d1f0:	83 ec 1c             	sub    $0x1c,%esp
   * removed.
   *
   * Avoid locking this list while walking it or else you will
   * introduce a potential deadlock in [at least] refill.c.
   */
  for (g = &ptr->__sglue; g != NULL; g = g->_next)
 804d1f3:	8b 7c 24 30          	mov    0x30(%esp),%edi
 804d1f7:	31 f6                	xor    %esi,%esi
 804d1f9:	81 c7 e0 02 00 00    	add    $0x2e0,%edi
 804d1ff:	74 37                	je     804d238 <_fwalk+0x4c>
 804d201:	8d 76 00             	lea    0x0(%esi),%esi
    for (fp = g->_iobs, n = g->_niobs; --n >= 0; fp++)
 804d204:	8b 5f 08             	mov    0x8(%edi),%ebx
 804d207:	8b 6f 04             	mov    0x4(%edi),%ebp
 804d20a:	4d                   	dec    %ebp
 804d20b:	78 25                	js     804d232 <_fwalk+0x46>
 804d20d:	8d 76 00             	lea    0x0(%esi),%esi
      if (fp->_flags != 0 && fp->_flags != 1 && fp->_file != -1)
 804d210:	66 83 7b 0c 01       	cmpw   $0x1,0xc(%ebx)
 804d215:	76 12                	jbe    804d229 <_fwalk+0x3d>
 804d217:	66 83 7b 0e ff       	cmpw   $0xffff,0xe(%ebx)
 804d21c:	74 0b                	je     804d229 <_fwalk+0x3d>
	ret |= (*function) (fp);
 804d21e:	89 1c 24             	mov    %ebx,(%esp)
 804d221:	8b 54 24 34          	mov    0x34(%esp),%edx
 804d225:	ff d2                	call   *%edx
 804d227:	09 c6                	or     %eax,%esi
   *
   * Avoid locking this list while walking it or else you will
   * introduce a potential deadlock in [at least] refill.c.
   */
  for (g = &ptr->__sglue; g != NULL; g = g->_next)
    for (fp = g->_iobs, n = g->_niobs; --n >= 0; fp++)
 804d229:	83 c3 68             	add    $0x68,%ebx
 804d22c:	4d                   	dec    %ebp
 804d22d:	83 fd ff             	cmp    $0xffffffff,%ebp
 804d230:	75 de                	jne    804d210 <_fwalk+0x24>
   * removed.
   *
   * Avoid locking this list while walking it or else you will
   * introduce a potential deadlock in [at least] refill.c.
   */
  for (g = &ptr->__sglue; g != NULL; g = g->_next)
 804d232:	8b 3f                	mov    (%edi),%edi
 804d234:	85 ff                	test   %edi,%edi
 804d236:	75 cc                	jne    804d204 <_fwalk+0x18>
    for (fp = g->_iobs, n = g->_niobs; --n >= 0; fp++)
      if (fp->_flags != 0 && fp->_flags != 1 && fp->_file != -1)
	ret |= (*function) (fp);

  return ret;
}
 804d238:	89 f0                	mov    %esi,%eax
 804d23a:	83 c4 1c             	add    $0x1c,%esp
 804d23d:	5b                   	pop    %ebx
 804d23e:	5e                   	pop    %esi
 804d23f:	5f                   	pop    %edi
 804d240:	5d                   	pop    %ebp
 804d241:	c3                   	ret    
 804d242:	66 90                	xchg   %ax,%ax

0804d244 <_fwalk_reent>:
   I/O function (e.g. _fclose_r).  */
int
_DEFUN(_fwalk_reent, (ptr, reent_function),
       struct _reent *ptr _AND
       register int (*reent_function) (struct _reent *, FILE *))
{
 804d244:	55                   	push   %ebp
 804d245:	57                   	push   %edi
 804d246:	56                   	push   %esi
 804d247:	53                   	push   %ebx
 804d248:	83 ec 1c             	sub    $0x1c,%esp
   * removed.
   *
   * Avoid locking this list while walking it or else you will
   * introduce a potential deadlock in [at least] refill.c.
   */
  for (g = &ptr->__sglue; g != NULL; g = g->_next)
 804d24b:	8b 6c 24 30          	mov    0x30(%esp),%ebp
 804d24f:	31 f6                	xor    %esi,%esi
 804d251:	81 c5 e0 02 00 00    	add    $0x2e0,%ebp
 804d257:	74 4b                	je     804d2a4 <_fwalk_reent+0x60>
 804d259:	8d 76 00             	lea    0x0(%esi),%esi
    for (fp = g->_iobs, n = g->_niobs; --n >= 0; fp++)
 804d25c:	8b 5d 08             	mov    0x8(%ebp),%ebx
 804d25f:	8b 7d 04             	mov    0x4(%ebp),%edi
 804d262:	4f                   	dec    %edi
 804d263:	78 38                	js     804d29d <_fwalk_reent+0x59>
 804d265:	8d 76 00             	lea    0x0(%esi),%esi
      if (fp->_flags != 0)
 804d268:	66 8b 43 0c          	mov    0xc(%ebx),%ax
 804d26c:	66 85 c0             	test   %ax,%ax
 804d26f:	74 23                	je     804d294 <_fwalk_reent+0x50>
        {
          if (fp->_flags != 0 && fp->_flags != 1 && fp->_file != -1)
 804d271:	66 83 f8 01          	cmp    $0x1,%ax
 804d275:	76 1d                	jbe    804d294 <_fwalk_reent+0x50>
 804d277:	66 83 7b 0e ff       	cmpw   $0xffff,0xe(%ebx)
 804d27c:	74 16                	je     804d294 <_fwalk_reent+0x50>
            ret |= (*reent_function) (ptr, fp);
 804d27e:	89 5c 24 04          	mov    %ebx,0x4(%esp)
 804d282:	8b 54 24 30          	mov    0x30(%esp),%edx
 804d286:	89 14 24             	mov    %edx,(%esp)
 804d289:	8b 54 24 34          	mov    0x34(%esp),%edx
 804d28d:	ff d2                	call   *%edx
 804d28f:	09 c6                	or     %eax,%esi
 804d291:	8d 76 00             	lea    0x0(%esi),%esi
   *
   * Avoid locking this list while walking it or else you will
   * introduce a potential deadlock in [at least] refill.c.
   */
  for (g = &ptr->__sglue; g != NULL; g = g->_next)
    for (fp = g->_iobs, n = g->_niobs; --n >= 0; fp++)
 804d294:	83 c3 68             	add    $0x68,%ebx
 804d297:	4f                   	dec    %edi
 804d298:	83 ff ff             	cmp    $0xffffffff,%edi
 804d29b:	75 cb                	jne    804d268 <_fwalk_reent+0x24>
   * removed.
   *
   * Avoid locking this list while walking it or else you will
   * introduce a potential deadlock in [at least] refill.c.
   */
  for (g = &ptr->__sglue; g != NULL; g = g->_next)
 804d29d:	8b 6d 00             	mov    0x0(%ebp),%ebp
 804d2a0:	85 ed                	test   %ebp,%ebp
 804d2a2:	75 b8                	jne    804d25c <_fwalk_reent+0x18>
          if (fp->_flags != 0 && fp->_flags != 1 && fp->_file != -1)
            ret |= (*reent_function) (ptr, fp);
        }

  return ret;
}
 804d2a4:	89 f0                	mov    %esi,%eax
 804d2a6:	83 c4 1c             	add    $0x1c,%esp
 804d2a9:	5b                   	pop    %ebx
 804d2aa:	5e                   	pop    %esi
 804d2ab:	5f                   	pop    %edi
 804d2ac:	5d                   	pop    %ebp
 804d2ad:	c3                   	ret    
 804d2ae:	90                   	nop
 804d2af:	90                   	nop

0804d2b0 <_setlocale_r>:
char *
_DEFUN(_setlocale_r, (p, category, locale),
       struct _reent *p _AND
       int category _AND
       _CONST char *locale)
{
 804d2b0:	53                   	push   %ebx
 804d2b1:	83 ec 18             	sub    $0x18,%esp
 804d2b4:	8b 5c 24 28          	mov    0x28(%esp),%ebx
#ifndef _MB_CAPABLE
  if (locale)
 804d2b8:	85 db                	test   %ebx,%ebx
 804d2ba:	74 14                	je     804d2d0 <_setlocale_r+0x20>
    { 
      if (strcmp (locale, "POSIX") && strcmp (locale, "C")
 804d2bc:	c7 44 24 04 98 18 05 	movl   $0x8051898,0x4(%esp)
 804d2c3:	08 
 804d2c4:	89 1c 24             	mov    %ebx,(%esp)
 804d2c7:	e8 c8 1f 00 00       	call   804f294 <strcmp>
 804d2cc:	85 c0                	test   %eax,%eax
 804d2ce:	75 0c                	jne    804d2dc <_setlocale_r+0x2c>
	  && strcmp (locale, ""))
        return NULL;
    }
  return "C";
 804d2d0:	b8 90 16 05 08       	mov    $0x8051690,%eax
	  return NULL;
	}
    }
  return currentlocale ();
#endif /* !_MB_CAPABLE */
}
 804d2d5:	83 c4 18             	add    $0x18,%esp
 804d2d8:	5b                   	pop    %ebx
 804d2d9:	c3                   	ret    
 804d2da:	66 90                	xchg   %ax,%ax
       _CONST char *locale)
{
#ifndef _MB_CAPABLE
  if (locale)
    { 
      if (strcmp (locale, "POSIX") && strcmp (locale, "C")
 804d2dc:	c7 44 24 04 90 16 05 	movl   $0x8051690,0x4(%esp)
 804d2e3:	08 
 804d2e4:	89 1c 24             	mov    %ebx,(%esp)
 804d2e7:	e8 a8 1f 00 00       	call   804f294 <strcmp>
 804d2ec:	85 c0                	test   %eax,%eax
 804d2ee:	74 e0                	je     804d2d0 <_setlocale_r+0x20>
	  && strcmp (locale, ""))
 804d2f0:	c7 44 24 04 93 16 05 	movl   $0x8051693,0x4(%esp)
 804d2f7:	08 
 804d2f8:	89 1c 24             	mov    %ebx,(%esp)
 804d2fb:	e8 94 1f 00 00       	call   804f294 <strcmp>
 804d300:	89 c2                	mov    %eax,%edx
        return NULL;
    }
  return "C";
 804d302:	b8 90 16 05 08       	mov    $0x8051690,%eax
{
#ifndef _MB_CAPABLE
  if (locale)
    { 
      if (strcmp (locale, "POSIX") && strcmp (locale, "C")
	  && strcmp (locale, ""))
 804d307:	85 d2                	test   %edx,%edx
 804d309:	74 ca                	je     804d2d5 <_setlocale_r+0x25>
        return NULL;
 804d30b:	31 c0                	xor    %eax,%eax
 804d30d:	eb c6                	jmp    804d2d5 <_setlocale_r+0x25>
 804d30f:	90                   	nop

0804d310 <__locale_charset>:
#if 0//def __HAVE_LOCALE_INFO__
  return __get_current_ctype_locale ()->codeset;
#else
  return lc_ctype_charset;
#endif
}
 804d310:	b8 c0 44 05 08       	mov    $0x80544c0,%eax
 804d315:	c3                   	ret    
 804d316:	66 90                	xchg   %ax,%ax

0804d318 <__locale_mb_cur_max>:
#if 0//def __HAVE_LOCALE_INFO__
  return __get_current_ctype_locale ()->mb_cur_max[0];
#else
  return __mb_cur_max;
#endif
}
 804d318:	a1 a0 44 05 08       	mov    0x80544a0,%eax
 804d31d:	c3                   	ret    
 804d31e:	66 90                	xchg   %ax,%ax

0804d320 <__locale_msgcharset>:
#ifdef __HAVE_LOCALE_INFO__
  return (char *) __get_current_messages_locale ()->codeset;
#else
  return lc_message_charset;
#endif
}
 804d320:	b8 e0 44 05 08       	mov    $0x80544e0,%eax
 804d325:	c3                   	ret    
 804d326:	66 90                	xchg   %ax,%ax

0804d328 <__locale_cjk_lang>:

int
_DEFUN_VOID(__locale_cjk_lang)
{
  return lc_ctype_cjk_lang;
}
 804d328:	31 c0                	xor    %eax,%eax
 804d32a:	c3                   	ret    
 804d32b:	90                   	nop

0804d32c <_localeconv_r>:
#endif /* !__HAVE_LOCALE_INFO_EXTENDED__ */
      __mlocale_changed = 0;
    }
#endif /* __HAVE_LOCALE_INFO__ */
  return (struct lconv *) &lconv;
}
 804d32c:	b8 00 45 05 08       	mov    $0x8054500,%eax
 804d331:	c3                   	ret    
 804d332:	66 90                	xchg   %ax,%ax

0804d334 <setlocale>:
   initialization work.  It calls _setlocale_r, though. */
char *
_DEFUN(setlocale, (category, locale),
       int category _AND
       _CONST char *locale)
{
 804d334:	83 ec 1c             	sub    $0x1c,%esp
  return _setlocale_r (_REENT, category, locale);
 804d337:	8b 44 24 24          	mov    0x24(%esp),%eax
 804d33b:	89 44 24 08          	mov    %eax,0x8(%esp)
 804d33f:	8b 44 24 20          	mov    0x20(%esp),%eax
 804d343:	89 44 24 04          	mov    %eax,0x4(%esp)
 804d347:	a1 40 40 05 08       	mov    0x8054040,%eax
 804d34c:	89 04 24             	mov    %eax,(%esp)
 804d34f:	e8 5c ff ff ff       	call   804d2b0 <_setlocale_r>
}
 804d354:	83 c4 1c             	add    $0x1c,%esp
 804d357:	c3                   	ret    

0804d358 <localeconv>:

struct lconv *
_DEFUN_VOID(localeconv)
{
  return _localeconv_r (_REENT);
}
 804d358:	b8 00 45 05 08       	mov    $0x8054500,%eax
 804d35d:	c3                   	ret    
 804d35e:	90                   	nop
 804d35f:	90                   	nop

0804d360 <__smakebuf_r>:

_VOID
_DEFUN(__smakebuf_r, (ptr, fp),
       struct _reent *ptr _AND
       register FILE *fp)
{
 804d360:	55                   	push   %ebp
 804d361:	57                   	push   %edi
 804d362:	56                   	push   %esi
 804d363:	53                   	push   %ebx
 804d364:	83 ec 5c             	sub    $0x5c,%esp
 804d367:	8b 7c 24 70          	mov    0x70(%esp),%edi
 804d36b:	8b 5c 24 74          	mov    0x74(%esp),%ebx
  struct stat64 st;
#else
  struct stat st;
#endif

  if (fp->_flags & __SNBF)
 804d36f:	8b 43 0c             	mov    0xc(%ebx),%eax
 804d372:	a8 02                	test   $0x2,%al
 804d374:	0f 85 b2 00 00 00    	jne    804d42c <__smakebuf_r+0xcc>
      return;
    }
#ifdef __USE_INTERNAL_STAT64
  if (fp->_file < 0 || _fstat64_r (ptr, fp->_file, &st) < 0)
#else
  if (fp->_file < 0 || _fstat_r (ptr, fp->_file, &st) < 0)
 804d37a:	0f bf 53 0e          	movswl 0xe(%ebx),%edx
 804d37e:	66 85 d2             	test   %dx,%dx
 804d381:	78 48                	js     804d3cb <__smakebuf_r+0x6b>
 804d383:	8d 44 24 14          	lea    0x14(%esp),%eax
 804d387:	89 44 24 08          	mov    %eax,0x8(%esp)
 804d38b:	89 54 24 04          	mov    %edx,0x4(%esp)
 804d38f:	89 3c 24             	mov    %edi,(%esp)
 804d392:	e8 bd 32 00 00       	call   8050654 <_fstat_r>
 804d397:	85 c0                	test   %eax,%eax
 804d399:	78 2d                	js     804d3c8 <__smakebuf_r+0x68>
      fp->_flags |= __SNPT;
#endif
    }
  else
    {
      couldbetty = (st.st_mode & S_IFMT) == S_IFCHR;
 804d39b:	8b 44 24 18          	mov    0x18(%esp),%eax
 804d39f:	25 00 f0 00 00       	and    $0xf000,%eax
 804d3a4:	31 d2                	xor    %edx,%edx
 804d3a6:	3d 00 20 00 00       	cmp    $0x2000,%eax
 804d3ab:	0f 94 c2             	sete   %dl
 804d3ae:	89 d5                	mov    %edx,%ebp
#ifdef _FSEEK_OPTIMIZATION
      /*
       * Optimize fseek() only if it is a regular file.
       * (The test for __sseek is mainly paranoia.)
       */
      if ((st.st_mode & S_IFMT) == S_IFREG && fp->_seek == __sseek)
 804d3b0:	3d 00 80 00 00       	cmp    $0x8000,%eax
 804d3b5:	0f 84 89 00 00 00    	je     804d444 <__smakebuf_r+0xe4>
#else
	  fp->_blksize = 1024;
#endif
	}
      else
	fp->_flags |= __SNPT;
 804d3bb:	66 81 4b 0c 00 08    	orw    $0x800,0xc(%ebx)
    {
      couldbetty = (st.st_mode & S_IFMT) == S_IFCHR;
#ifdef HAVE_BLKSIZE
      size = st.st_blksize <= 0 ? BUFSIZ : st.st_blksize;
#else
      size = BUFSIZ;
 804d3c1:	be 00 04 00 00       	mov    $0x400,%esi
 804d3c6:	eb 23                	jmp    804d3eb <__smakebuf_r+0x8b>
 804d3c8:	8b 43 0c             	mov    0xc(%ebx),%eax
  if (fp->_file < 0 || _fstat_r (ptr, fp->_file, &st) < 0)
#endif
    {
      couldbetty = 0;
      /* Check if we are be called by asprintf family for initial buffer.  */
      if (fp->_flags & __SMBF)
 804d3cb:	89 c2                	mov    %eax,%edx
 804d3cd:	81 e2 80 00 00 00    	and    $0x80,%edx
        size = _DEFAULT_ASPRINTF_BUFSIZE;
 804d3d3:	66 83 fa 01          	cmp    $0x1,%dx
 804d3d7:	19 f6                	sbb    %esi,%esi
 804d3d9:	81 e6 c0 03 00 00    	and    $0x3c0,%esi
 804d3df:	83 c6 40             	add    $0x40,%esi
      else
        size = BUFSIZ;
#ifdef _FSEEK_OPTIMIZATION
      /* do not try to optimise fseek() */
      fp->_flags |= __SNPT;
 804d3e2:	80 cc 08             	or     $0x8,%ah
 804d3e5:	66 89 43 0c          	mov    %ax,0xc(%ebx)
  if (fp->_file < 0 || _fstat64_r (ptr, fp->_file, &st) < 0)
#else
  if (fp->_file < 0 || _fstat_r (ptr, fp->_file, &st) < 0)
#endif
    {
      couldbetty = 0;
 804d3e9:	31 ed                	xor    %ebp,%ebp
	}
      else
	fp->_flags |= __SNPT;
#endif
    }
  if ((p = _malloc_r (ptr, size)) == NULL)
 804d3eb:	89 74 24 04          	mov    %esi,0x4(%esp)
 804d3ef:	89 3c 24             	mov    %edi,(%esp)
 804d3f2:	e8 c5 00 00 00       	call   804d4bc <_malloc_r>
 804d3f7:	85 c0                	test   %eax,%eax
 804d3f9:	74 21                	je     804d41c <__smakebuf_r+0xbc>
	  fp->_bf._size = 1;
	}
    }
  else
    {
      ptr->__cleanup = _cleanup_r;
 804d3fb:	c7 47 3c 4c c7 04 08 	movl   $0x804c74c,0x3c(%edi)
      fp->_flags |= __SMBF;
 804d402:	66 81 4b 0c 80 00    	orw    $0x80,0xc(%ebx)
      fp->_bf._base = fp->_p = (unsigned char *) p;
 804d408:	89 03                	mov    %eax,(%ebx)
 804d40a:	89 43 10             	mov    %eax,0x10(%ebx)
      fp->_bf._size = size;
 804d40d:	89 73 14             	mov    %esi,0x14(%ebx)
      if (couldbetty && _isatty_r (ptr, fp->_file))
 804d410:	85 ed                	test   %ebp,%ebp
 804d412:	75 54                	jne    804d468 <__smakebuf_r+0x108>
	fp->_flags |= __SLBF;
    }
}
 804d414:	83 c4 5c             	add    $0x5c,%esp
 804d417:	5b                   	pop    %ebx
 804d418:	5e                   	pop    %esi
 804d419:	5f                   	pop    %edi
 804d41a:	5d                   	pop    %ebp
 804d41b:	c3                   	ret    
	fp->_flags |= __SNPT;
#endif
    }
  if ((p = _malloc_r (ptr, size)) == NULL)
    {
      if (!(fp->_flags & __SSTR))
 804d41c:	8b 43 0c             	mov    0xc(%ebx),%eax
 804d41f:	f6 c4 02             	test   $0x2,%ah
 804d422:	75 f0                	jne    804d414 <__smakebuf_r+0xb4>
	{
	  fp->_flags |= __SNBF;
 804d424:	83 c8 02             	or     $0x2,%eax
 804d427:	66 89 43 0c          	mov    %ax,0xc(%ebx)
 804d42b:	90                   	nop
	  fp->_bf._base = fp->_p = fp->_nbuf;
 804d42c:	8d 43 43             	lea    0x43(%ebx),%eax
 804d42f:	89 03                	mov    %eax,(%ebx)
 804d431:	89 43 10             	mov    %eax,0x10(%ebx)
	  fp->_bf._size = 1;
 804d434:	c7 43 14 01 00 00 00 	movl   $0x1,0x14(%ebx)
      fp->_bf._base = fp->_p = (unsigned char *) p;
      fp->_bf._size = size;
      if (couldbetty && _isatty_r (ptr, fp->_file))
	fp->_flags |= __SLBF;
    }
}
 804d43b:	83 c4 5c             	add    $0x5c,%esp
 804d43e:	5b                   	pop    %ebx
 804d43f:	5e                   	pop    %esi
 804d440:	5f                   	pop    %edi
 804d441:	5d                   	pop    %ebp
 804d442:	c3                   	ret    
 804d443:	90                   	nop
#ifdef _FSEEK_OPTIMIZATION
      /*
       * Optimize fseek() only if it is a regular file.
       * (The test for __sseek is mainly paranoia.)
       */
      if ((st.st_mode & S_IFMT) == S_IFREG && fp->_seek == __sseek)
 804d444:	81 7b 28 34 f2 04 08 	cmpl   $0x804f234,0x28(%ebx)
 804d44b:	0f 85 6a ff ff ff    	jne    804d3bb <__smakebuf_r+0x5b>
	{
	  fp->_flags |= __SOPT;
 804d451:	66 81 4b 0c 00 04    	orw    $0x400,0xc(%ebx)
#ifdef HAVE_BLKSIZE
	  fp->_blksize = st.st_blksize;
#else
	  fp->_blksize = 1024;
 804d457:	c7 43 4c 00 04 00 00 	movl   $0x400,0x4c(%ebx)
    {
      couldbetty = (st.st_mode & S_IFMT) == S_IFCHR;
#ifdef HAVE_BLKSIZE
      size = st.st_blksize <= 0 ? BUFSIZ : st.st_blksize;
#else
      size = BUFSIZ;
 804d45e:	be 00 04 00 00       	mov    $0x400,%esi
	{
	  fp->_flags |= __SOPT;
#ifdef HAVE_BLKSIZE
	  fp->_blksize = st.st_blksize;
#else
	  fp->_blksize = 1024;
 804d463:	eb 86                	jmp    804d3eb <__smakebuf_r+0x8b>
 804d465:	8d 76 00             	lea    0x0(%esi),%esi
    {
      ptr->__cleanup = _cleanup_r;
      fp->_flags |= __SMBF;
      fp->_bf._base = fp->_p = (unsigned char *) p;
      fp->_bf._size = size;
      if (couldbetty && _isatty_r (ptr, fp->_file))
 804d468:	0f bf 43 0e          	movswl 0xe(%ebx),%eax
 804d46c:	89 44 24 04          	mov    %eax,0x4(%esp)
 804d470:	89 3c 24             	mov    %edi,(%esp)
 804d473:	e8 1c 32 00 00       	call   8050694 <_isatty_r>
 804d478:	85 c0                	test   %eax,%eax
 804d47a:	74 98                	je     804d414 <__smakebuf_r+0xb4>
	fp->_flags |= __SLBF;
 804d47c:	66 83 4b 0c 01       	orw    $0x1,0xc(%ebx)
 804d481:	eb 91                	jmp    804d414 <__smakebuf_r+0xb4>
 804d483:	90                   	nop

0804d484 <malloc>:
#ifndef _REENT_ONLY

_PTR
_DEFUN (malloc, (nbytes),
	size_t nbytes)		/* get a block */
{
 804d484:	83 ec 1c             	sub    $0x1c,%esp
  return _malloc_r (_REENT, nbytes);
 804d487:	8b 44 24 20          	mov    0x20(%esp),%eax
 804d48b:	89 44 24 04          	mov    %eax,0x4(%esp)
 804d48f:	a1 40 40 05 08       	mov    0x8054040,%eax
 804d494:	89 04 24             	mov    %eax,(%esp)
 804d497:	e8 20 00 00 00       	call   804d4bc <_malloc_r>
}
 804d49c:	83 c4 1c             	add    $0x1c,%esp
 804d49f:	c3                   	ret    

0804d4a0 <free>:

void
_DEFUN (free, (aptr),
	_PTR aptr)
{
 804d4a0:	83 ec 1c             	sub    $0x1c,%esp
  _free_r (_REENT, aptr);
 804d4a3:	8b 44 24 20          	mov    0x20(%esp),%eax
 804d4a7:	89 44 24 04          	mov    %eax,0x4(%esp)
 804d4ab:	a1 40 40 05 08       	mov    0x8054040,%eax
 804d4b0:	89 04 24             	mov    %eax,(%esp)
 804d4b3:	e8 48 f6 ff ff       	call   804cb00 <_free_r>
}
 804d4b8:	83 c4 1c             	add    $0x1c,%esp
 804d4bb:	c3                   	ret    

0804d4bc <_malloc_r>:
#if __STD_C
Void_t* mALLOc(RARG size_t bytes)
#else
Void_t* mALLOc(RARG bytes) RDECL size_t bytes;
#endif
{
 804d4bc:	55                   	push   %ebp
 804d4bd:	57                   	push   %edi
 804d4be:	56                   	push   %esi
 804d4bf:	53                   	push   %ebx
 804d4c0:	83 ec 2c             	sub    $0x2c,%esp
 804d4c3:	8b 44 24 44          	mov    0x44(%esp),%eax
  int       startidx;                /* first bin of a traversed block */
  mchunkptr fwd;                     /* misc temp for linking */
  mchunkptr bck;                     /* misc temp for linking */
  mbinptr q;                         /* misc temp */

  INTERNAL_SIZE_T nb  = request2size(bytes);  /* padded request size; */
 804d4c7:	8d 58 0b             	lea    0xb(%eax),%ebx
 804d4ca:	83 fb 16             	cmp    $0x16,%ebx
 804d4cd:	76 65                	jbe    804d534 <_malloc_r+0x78>

  /* Check for overflow and just fail, if so. */
  if (nb > INT_MAX || nb < bytes)
 804d4cf:	83 e3 f8             	and    $0xfffffff8,%ebx
 804d4d2:	78 69                	js     804d53d <_malloc_r+0x81>
 804d4d4:	39 d8                	cmp    %ebx,%eax
 804d4d6:	77 65                	ja     804d53d <_malloc_r+0x81>
  {
    RERRNO = ENOMEM;
    return 0;
  }

  MALLOC_LOCK;
 804d4d8:	8b 44 24 40          	mov    0x40(%esp),%eax
 804d4dc:	89 04 24             	mov    %eax,(%esp)
 804d4df:	e8 c4 08 00 00       	call   804dda8 <__malloc_lock>

  /* Check for exact match in a bin */

  if (is_small_request(nb))  /* Faster version for small requests */
 804d4e4:	81 fb f7 01 00 00    	cmp    $0x1f7,%ebx
 804d4ea:	77 68                	ja     804d554 <_malloc_r+0x98>
  {
    idx = smallbin_index(nb); 
 804d4ec:	89 da                	mov    %ebx,%edx
 804d4ee:	c1 ea 03             	shr    $0x3,%edx

    /* No traversal or size check necessary for small bins.  */

    q = bin_at(idx);
 804d4f1:	8d 04 d5 60 45 05 08 	lea    0x8054560(,%edx,8),%eax
    victim = last(q);
 804d4f8:	8b 70 0c             	mov    0xc(%eax),%esi

#if MALLOC_ALIGN != 16
    /* Also scan the next one, since it would have a remainder < MINSIZE */
    if (victim == q)
 804d4fb:	39 c6                	cmp    %eax,%esi
 804d4fd:	0f 84 c9 04 00 00    	je     804d9cc <_malloc_r+0x510>
      victim = last(q);
    }
#endif
    if (victim != q)
    {
      victim_size = chunksize(victim);
 804d503:	8b 46 04             	mov    0x4(%esi),%eax
 804d506:	83 e0 fc             	and    $0xfffffffc,%eax
        break;   
      }

      else if (remainder_size >= 0) /* exact fit */
      {
        unlink(victim, bck, fwd);
 804d509:	8b 56 0c             	mov    0xc(%esi),%edx
 804d50c:	8b 4e 08             	mov    0x8(%esi),%ecx
 804d50f:	89 51 0c             	mov    %edx,0xc(%ecx)
 804d512:	89 4a 08             	mov    %ecx,0x8(%edx)
        set_inuse_bit_at_offset(victim, victim_size);
 804d515:	83 4c 06 04 01       	orl    $0x1,0x4(%esi,%eax,1)
        check_malloced_chunk(victim, nb);
	MALLOC_UNLOCK;
 804d51a:	8b 44 24 40          	mov    0x40(%esp),%eax
 804d51e:	89 04 24             	mov    %eax,(%esp)
 804d521:	e8 86 08 00 00       	call   804ddac <__malloc_unlock>
        return chunk2mem(victim);
 804d526:	8d 46 08             	lea    0x8(%esi),%eax
  check_malloced_chunk(victim, nb);
  MALLOC_UNLOCK;
  return chunk2mem(victim);

#endif /* MALLOC_PROVIDED */
}
 804d529:	83 c4 2c             	add    $0x2c,%esp
 804d52c:	5b                   	pop    %ebx
 804d52d:	5e                   	pop    %esi
 804d52e:	5f                   	pop    %edi
 804d52f:	5d                   	pop    %ebp
 804d530:	c3                   	ret    
 804d531:	8d 76 00             	lea    0x0(%esi),%esi
  int       startidx;                /* first bin of a traversed block */
  mchunkptr fwd;                     /* misc temp for linking */
  mchunkptr bck;                     /* misc temp for linking */
  mbinptr q;                         /* misc temp */

  INTERNAL_SIZE_T nb  = request2size(bytes);  /* padded request size; */
 804d534:	bb 10 00 00 00       	mov    $0x10,%ebx

  /* Check for overflow and just fail, if so. */
  if (nb > INT_MAX || nb < bytes)
 804d539:	39 d8                	cmp    %ebx,%eax
 804d53b:	76 9b                	jbe    804d4d8 <_malloc_r+0x1c>
  {
    RERRNO = ENOMEM;
 804d53d:	8b 44 24 40          	mov    0x40(%esp),%eax
 804d541:	c7 00 0c 00 00 00    	movl   $0xc,(%eax)
    return 0;
 804d547:	31 c0                	xor    %eax,%eax
  check_malloced_chunk(victim, nb);
  MALLOC_UNLOCK;
  return chunk2mem(victim);

#endif /* MALLOC_PROVIDED */
}
 804d549:	83 c4 2c             	add    $0x2c,%esp
 804d54c:	5b                   	pop    %ebx
 804d54d:	5e                   	pop    %esi
 804d54e:	5f                   	pop    %edi
 804d54f:	5d                   	pop    %ebp
 804d550:	c3                   	ret    
 804d551:	8d 76 00             	lea    0x0(%esi),%esi
    idx += 2; /* Set for bin scan below. We've already scanned 2 bins. */

  }
  else
  {
    idx = bin_index(nb);
 804d554:	89 df                	mov    %ebx,%edi
 804d556:	c1 ef 09             	shr    $0x9,%edi
 804d559:	0f 84 81 01 00 00    	je     804d6e0 <_malloc_r+0x224>
 804d55f:	83 ff 04             	cmp    $0x4,%edi
 804d562:	0f 87 3c 03 00 00    	ja     804d8a4 <_malloc_r+0x3e8>
 804d568:	89 df                	mov    %ebx,%edi
 804d56a:	c1 ef 06             	shr    $0x6,%edi
 804d56d:	83 c7 38             	add    $0x38,%edi
 804d570:	8d 04 3f             	lea    (%edi,%edi,1),%eax
    bin = bin_at(idx);
 804d573:	8d 0c 85 60 45 05 08 	lea    0x8054560(,%eax,4),%ecx

    for (victim = last(bin); victim != bin; victim = victim->bk)
 804d57a:	8b 71 0c             	mov    0xc(%ecx),%esi
 804d57d:	39 f1                	cmp    %esi,%ecx
 804d57f:	75 0e                	jne    804d58f <_malloc_r+0xd3>
 804d581:	eb 1c                	jmp    804d59f <_malloc_r+0xe3>
 804d583:	90                   	nop
      {
        --idx; /* adjust to rescan below after checking last remainder */
        break;   
      }

      else if (remainder_size >= 0) /* exact fit */
 804d584:	85 d2                	test   %edx,%edx
 804d586:	79 81                	jns    804d509 <_malloc_r+0x4d>
  else
  {
    idx = bin_index(nb);
    bin = bin_at(idx);

    for (victim = last(bin); victim != bin; victim = victim->bk)
 804d588:	8b 76 0c             	mov    0xc(%esi),%esi
 804d58b:	39 f1                	cmp    %esi,%ecx
 804d58d:	74 10                	je     804d59f <_malloc_r+0xe3>
    {
      victim_size = chunksize(victim);
 804d58f:	8b 46 04             	mov    0x4(%esi),%eax
 804d592:	83 e0 fc             	and    $0xfffffffc,%eax
      remainder_size = long_sub_size_t(victim_size, nb);
 804d595:	89 c2                	mov    %eax,%edx
 804d597:	29 da                	sub    %ebx,%edx
      
      if (remainder_size >= (long)MINSIZE) /* too big */
 804d599:	83 fa 0f             	cmp    $0xf,%edx
 804d59c:	7e e6                	jle    804d584 <_malloc_r+0xc8>
      {
        --idx; /* adjust to rescan below after checking last remainder */
 804d59e:	4f                   	dec    %edi
	MALLOC_UNLOCK;
        return chunk2mem(victim);
      }
    }

    ++idx; 
 804d59f:	8d 6f 01             	lea    0x1(%edi),%ebp

  }

  /* Try to use the last split-off remainder */

  if ( (victim = last_remainder->fd) != last_remainder)
 804d5a2:	8b 35 70 45 05 08    	mov    0x8054570,%esi
 804d5a8:	81 fe 68 45 05 08    	cmp    $0x8054568,%esi
 804d5ae:	0f 84 b4 03 00 00    	je     804d968 <_malloc_r+0x4ac>
  {
    victim_size = chunksize(victim);
 804d5b4:	8b 7e 04             	mov    0x4(%esi),%edi
 804d5b7:	83 e7 fc             	and    $0xfffffffc,%edi
    remainder_size = long_sub_size_t(victim_size, nb);
 804d5ba:	89 f8                	mov    %edi,%eax
 804d5bc:	29 d8                	sub    %ebx,%eax

    if (remainder_size >= (long)MINSIZE) /* re-split */
 804d5be:	83 f8 0f             	cmp    $0xf,%eax
 804d5c1:	0f 8f 5d 03 00 00    	jg     804d924 <_malloc_r+0x468>
      check_malloced_chunk(victim, nb);
      MALLOC_UNLOCK;
      return chunk2mem(victim);
    }

    clear_last_remainder;
 804d5c7:	c7 05 74 45 05 08 68 	movl   $0x8054568,0x8054574
 804d5ce:	45 05 08 
 804d5d1:	c7 05 70 45 05 08 68 	movl   $0x8054568,0x8054570
 804d5d8:	45 05 08 

    if (remainder_size >= 0)  /* exhaust */
 804d5db:	85 c0                	test   %eax,%eax
 804d5dd:	0f 89 0d 01 00 00    	jns    804d6f0 <_malloc_r+0x234>
      return chunk2mem(victim);
    }

    /* Else place in bin */

    frontlink(victim, victim_size, remainder_index, bck, fwd);
 804d5e3:	81 ff ff 01 00 00    	cmp    $0x1ff,%edi
 804d5e9:	0f 87 d9 02 00 00    	ja     804d8c8 <_malloc_r+0x40c>
 804d5ef:	c1 ef 03             	shr    $0x3,%edi
 804d5f2:	89 f9                	mov    %edi,%ecx
 804d5f4:	c1 f9 02             	sar    $0x2,%ecx
 804d5f7:	b8 01 00 00 00       	mov    $0x1,%eax
 804d5fc:	d3 e0                	shl    %cl,%eax
 804d5fe:	0b 05 64 45 05 08    	or     0x8054564,%eax
 804d604:	a3 64 45 05 08       	mov    %eax,0x8054564
 804d609:	8d 14 fd 60 45 05 08 	lea    0x8054560(,%edi,8),%edx
 804d610:	8b 4a 08             	mov    0x8(%edx),%ecx
 804d613:	89 56 0c             	mov    %edx,0xc(%esi)
 804d616:	89 4e 08             	mov    %ecx,0x8(%esi)
 804d619:	89 72 08             	mov    %esi,0x8(%edx)
 804d61c:	89 71 0c             	mov    %esi,0xc(%ecx)
  /* 
     If there are any possibly nonempty big-enough blocks, 
     search for best fitting chunk by scanning bins in blockwidth units.
  */

  if ( (block = idx2binblock(idx)) <= binblocks)  
 804d61f:	89 e9                	mov    %ebp,%ecx
 804d621:	c1 f9 02             	sar    $0x2,%ecx
 804d624:	bf 01 00 00 00       	mov    $0x1,%edi
 804d629:	d3 e7                	shl    %cl,%edi
 804d62b:	39 c7                	cmp    %eax,%edi
 804d62d:	0f 87 d9 00 00 00    	ja     804d70c <_malloc_r+0x250>
  {

    /* Get to the first marked block */

    if ( (block & binblocks) == 0) 
 804d633:	85 f8                	test   %edi,%eax
 804d635:	75 1a                	jne    804d651 <_malloc_r+0x195>
    {
      /* force to an even block boundary */
      idx = (idx & ~(BINBLOCKWIDTH - 1)) + BINBLOCKWIDTH;
 804d637:	89 ea                	mov    %ebp,%edx
 804d639:	83 e2 fc             	and    $0xfffffffc,%edx
 804d63c:	8d 6a 04             	lea    0x4(%edx),%ebp
      block <<= 1;
 804d63f:	d1 e7                	shl    %edi
      while ((block & binblocks) == 0)
 804d641:	85 f8                	test   %edi,%eax
 804d643:	75 0c                	jne    804d651 <_malloc_r+0x195>
 804d645:	8d 76 00             	lea    0x0(%esi),%esi
      {
        idx += BINBLOCKWIDTH;
 804d648:	83 c5 04             	add    $0x4,%ebp
        block <<= 1;
 804d64b:	d1 e7                	shl    %edi
    if ( (block & binblocks) == 0) 
    {
      /* force to an even block boundary */
      idx = (idx & ~(BINBLOCKWIDTH - 1)) + BINBLOCKWIDTH;
      block <<= 1;
      while ((block & binblocks) == 0)
 804d64d:	85 f8                	test   %edi,%eax
 804d64f:	74 f7                	je     804d648 <_malloc_r+0x18c>
      
    /* For each possibly nonempty block ... */
    for (;;)  
    {
      startidx = idx;          /* (track incomplete blocks) */
      q = bin = bin_at(idx);
 804d651:	8d 04 ed 60 45 05 08 	lea    0x8054560(,%ebp,8),%eax
 804d658:	89 44 24 1c          	mov    %eax,0x1c(%esp)
 804d65c:	89 c1                	mov    %eax,%ecx
 804d65e:	89 6c 24 18          	mov    %ebp,0x18(%esp)
      /* For each bin in this block ... */
      do
      {
        /* Find and use first big enough chunk ... */

        for (victim = last(bin); victim != bin; victim = victim->bk)
 804d662:	8b 71 0c             	mov    0xc(%ecx),%esi
 804d665:	39 f1                	cmp    %esi,%ecx
 804d667:	75 1a                	jne    804d683 <_malloc_r+0x1c7>
 804d669:	e9 06 03 00 00       	jmp    804d974 <_malloc_r+0x4b8>
 804d66e:	66 90                	xchg   %ax,%ax
            check_malloced_chunk(victim, nb);
	    MALLOC_UNLOCK;
            return chunk2mem(victim);
          }

          else if (remainder_size >= 0)  /* take */
 804d670:	85 d2                	test   %edx,%edx
 804d672:	0f 89 6c 03 00 00    	jns    804d9e4 <_malloc_r+0x528>
      /* For each bin in this block ... */
      do
      {
        /* Find and use first big enough chunk ... */

        for (victim = last(bin); victim != bin; victim = victim->bk)
 804d678:	8b 76 0c             	mov    0xc(%esi),%esi
 804d67b:	39 f1                	cmp    %esi,%ecx
 804d67d:	0f 84 f1 02 00 00    	je     804d974 <_malloc_r+0x4b8>
        {
          victim_size = chunksize(victim);
 804d683:	8b 46 04             	mov    0x4(%esi),%eax
 804d686:	83 e0 fc             	and    $0xfffffffc,%eax
          remainder_size = long_sub_size_t(victim_size, nb);
 804d689:	89 c2                	mov    %eax,%edx
 804d68b:	29 da                	sub    %ebx,%edx

          if (remainder_size >= (long)MINSIZE) /* split */
 804d68d:	83 fa 0f             	cmp    $0xf,%edx
 804d690:	7e de                	jle    804d670 <_malloc_r+0x1b4>
          {
            remainder = chunk_at_offset(victim, nb);
 804d692:	8d 04 1e             	lea    (%esi,%ebx,1),%eax
            set_head(victim, nb | PREV_INUSE);
 804d695:	83 cb 01             	or     $0x1,%ebx
 804d698:	89 5e 04             	mov    %ebx,0x4(%esi)
            unlink(victim, bck, fwd);
 804d69b:	8b 4e 0c             	mov    0xc(%esi),%ecx
 804d69e:	8b 5e 08             	mov    0x8(%esi),%ebx
 804d6a1:	89 4b 0c             	mov    %ecx,0xc(%ebx)
 804d6a4:	89 59 08             	mov    %ebx,0x8(%ecx)
            link_last_remainder(remainder);
 804d6a7:	a3 74 45 05 08       	mov    %eax,0x8054574
 804d6ac:	a3 70 45 05 08       	mov    %eax,0x8054570
 804d6b1:	c7 40 0c 68 45 05 08 	movl   $0x8054568,0xc(%eax)
 804d6b8:	c7 40 08 68 45 05 08 	movl   $0x8054568,0x8(%eax)
            set_head(remainder, remainder_size | PREV_INUSE);
 804d6bf:	89 d1                	mov    %edx,%ecx
 804d6c1:	83 c9 01             	or     $0x1,%ecx
 804d6c4:	89 48 04             	mov    %ecx,0x4(%eax)
            set_foot(remainder, remainder_size);
 804d6c7:	89 14 10             	mov    %edx,(%eax,%edx,1)
            check_malloced_chunk(victim, nb);
	    MALLOC_UNLOCK;
 804d6ca:	8b 44 24 40          	mov    0x40(%esp),%eax
 804d6ce:	89 04 24             	mov    %eax,(%esp)
 804d6d1:	e8 d6 06 00 00       	call   804ddac <__malloc_unlock>
            return chunk2mem(victim);
 804d6d6:	8d 46 08             	lea    0x8(%esi),%eax
 804d6d9:	e9 4b fe ff ff       	jmp    804d529 <_malloc_r+0x6d>
 804d6de:	66 90                	xchg   %ax,%ax
    idx += 2; /* Set for bin scan below. We've already scanned 2 bins. */

  }
  else
  {
    idx = bin_index(nb);
 804d6e0:	89 df                	mov    %ebx,%edi
 804d6e2:	c1 ef 03             	shr    $0x3,%edi
 804d6e5:	8d 04 3f             	lea    (%edi,%edi,1),%eax
 804d6e8:	e9 86 fe ff ff       	jmp    804d573 <_malloc_r+0xb7>
 804d6ed:	8d 76 00             	lea    0x0(%esi),%esi

    clear_last_remainder;

    if (remainder_size >= 0)  /* exhaust */
    {
      set_inuse_bit_at_offset(victim, victim_size);
 804d6f0:	83 4c 3e 04 01       	orl    $0x1,0x4(%esi,%edi,1)
      check_malloced_chunk(victim, nb);
      MALLOC_UNLOCK;
 804d6f5:	8b 44 24 40          	mov    0x40(%esp),%eax
 804d6f9:	89 04 24             	mov    %eax,(%esp)
 804d6fc:	e8 ab 06 00 00       	call   804ddac <__malloc_unlock>
      return chunk2mem(victim);
 804d701:	8d 46 08             	lea    0x8(%esi),%eax
  check_malloced_chunk(victim, nb);
  MALLOC_UNLOCK;
  return chunk2mem(victim);

#endif /* MALLOC_PROVIDED */
}
 804d704:	83 c4 2c             	add    $0x2c,%esp
 804d707:	5b                   	pop    %ebx
 804d708:	5e                   	pop    %esi
 804d709:	5f                   	pop    %edi
 804d70a:	5d                   	pop    %ebp
 804d70b:	c3                   	ret    


  /* Try to use top chunk */

  /* Require that there be a remainder, ensuring top always exists  */
  remainder_size = long_sub_size_t(chunksize(top), nb);
 804d70c:	8b 35 68 45 05 08    	mov    0x8054568,%esi
 804d712:	8b 7e 04             	mov    0x4(%esi),%edi
 804d715:	83 e7 fc             	and    $0xfffffffc,%edi
  if (chunksize(top) < nb || remainder_size < (long)MINSIZE)
 804d718:	39 df                	cmp    %ebx,%edi
 804d71a:	72 0d                	jb     804d729 <_malloc_r+0x26d>


  /* Try to use top chunk */

  /* Require that there be a remainder, ensuring top always exists  */
  remainder_size = long_sub_size_t(chunksize(top), nb);
 804d71c:	89 f8                	mov    %edi,%eax
 804d71e:	29 d8                	sub    %ebx,%eax
  if (chunksize(top) < nb || remainder_size < (long)MINSIZE)
 804d720:	83 f8 0f             	cmp    $0xf,%eax
 804d723:	0f 8f 4b 01 00 00    	jg     804d874 <_malloc_r+0x3b8>
  char*     new_brk;              /* return of 2nd sbrk call */
  INTERNAL_SIZE_T top_size;       /* new size of top chunk */

  mchunkptr old_top     = top;  /* Record state of old top */
  INTERNAL_SIZE_T old_top_size = chunksize(old_top);
  char*     old_end      = (char*)(chunk_at_offset(old_top, old_top_size));
 804d729:	8d 04 3e             	lea    (%esi,%edi,1),%eax
 804d72c:	89 44 24 18          	mov    %eax,0x18(%esp)

  /* Pad request with top_pad plus minimal overhead */
  
  INTERNAL_SIZE_T    sbrk_size     = nb + top_pad + MINSIZE;
 804d730:	8b 0d f0 49 05 08    	mov    0x80549f0,%ecx
 804d736:	01 d9                	add    %ebx,%ecx

  /* If not the first time through, round to preserve page boundary */
  /* Otherwise, we need to correct to a page size below anyway. */
  /* (We also correct below if an intervening foreign sbrk call.) */

  if (sbrk_base != (char*)(-1))
 804d738:	83 3d 40 45 05 08 ff 	cmpl   $0xffffffff,0x8054540
 804d73f:	0f 84 a5 03 00 00    	je     804daea <_malloc_r+0x62e>
    sbrk_size = (sbrk_size + (pagesz - 1)) & ~(pagesz - 1);
 804d745:	8d a9 0f 10 00 00    	lea    0x100f(%ecx),%ebp
 804d74b:	81 e5 00 f0 ff ff    	and    $0xfffff000,%ebp

  brk = (char*)(MORECORE (sbrk_size));
 804d751:	89 6c 24 04          	mov    %ebp,0x4(%esp)
 804d755:	8b 44 24 40          	mov    0x40(%esp),%eax
 804d759:	89 04 24             	mov    %eax,(%esp)
 804d75c:	e8 6b 19 00 00       	call   804f0cc <_sbrk_r>
 804d761:	89 c1                	mov    %eax,%ecx

  /* Fail if sbrk failed or if a foreign sbrk call killed our space */
  if (brk == (char*)(MORECORE_FAILURE) || 
 804d763:	83 f8 ff             	cmp    $0xffffffff,%eax
 804d766:	0f 84 c7 02 00 00    	je     804da33 <_malloc_r+0x577>
 804d76c:	39 44 24 18          	cmp    %eax,0x18(%esp)
 804d770:	0f 87 b1 02 00 00    	ja     804da27 <_malloc_r+0x56b>
      (brk < old_end && old_top != initial_top))
    return;

  sbrked_mem += sbrk_size;
 804d776:	8b 15 c0 49 05 08    	mov    0x80549c0,%edx
 804d77c:	01 ea                	add    %ebp,%edx
 804d77e:	89 15 c0 49 05 08    	mov    %edx,0x80549c0

  if (brk == old_end /* can just add bytes to current top, unless
 804d784:	39 4c 24 18          	cmp    %ecx,0x18(%esp)
 804d788:	0f 84 79 03 00 00    	je     804db07 <_malloc_r+0x64b>
    top_size = sbrk_size + old_top_size;
    set_head(top, top_size | PREV_INUSE);
  }
  else
  {
    if (sbrk_base == (char*)(-1))  /* First time through. Record base */
 804d78e:	83 3d 40 45 05 08 ff 	cmpl   $0xffffffff,0x8054540
 804d795:	0f 84 9e 03 00 00    	je     804db39 <_malloc_r+0x67d>
      sbrk_base = brk;
    else  /* Someone else called sbrk().  Count those bytes as sbrked_mem. */
      sbrked_mem += brk - (char*)old_end;
 804d79b:	89 c8                	mov    %ecx,%eax
 804d79d:	2b 44 24 18          	sub    0x18(%esp),%eax
 804d7a1:	01 d0                	add    %edx,%eax
 804d7a3:	a3 c0 49 05 08       	mov    %eax,0x80549c0

    /* Guarantee alignment of first new chunk made from this space */
    front_misalign = (POINTER_UINT)chunk2mem(brk) & MALLOC_ALIGN_MASK;
    if (front_misalign > 0) 
 804d7a8:	89 c8                	mov    %ecx,%eax
 804d7aa:	83 e0 07             	and    $0x7,%eax
 804d7ad:	0f 84 ec 02 00 00    	je     804da9f <_malloc_r+0x5e3>
    {
      correction = (MALLOC_ALIGNMENT) - front_misalign;
 804d7b3:	89 ca                	mov    %ecx,%edx
 804d7b5:	29 c2                	sub    %eax,%edx
      brk += correction;
 804d7b7:	8d 4a 08             	lea    0x8(%edx),%ecx
 804d7ba:	ba 08 10 00 00       	mov    $0x1008,%edx
 804d7bf:	29 c2                	sub    %eax,%edx
    }
    else
      correction = 0;

    /* Guarantee the next brk will be at a page boundary */
    correction += pagesz - ((POINTER_UINT)(brk + sbrk_size) & (pagesz - 1));
 804d7c1:	8d 04 29             	lea    (%ecx,%ebp,1),%eax
 804d7c4:	25 ff 0f 00 00       	and    $0xfff,%eax
 804d7c9:	29 c2                	sub    %eax,%edx

    /* Allocate correction */
    new_brk = (char*)(MORECORE (correction));
 804d7cb:	89 54 24 04          	mov    %edx,0x4(%esp)
 804d7cf:	8b 44 24 40          	mov    0x40(%esp),%eax
 804d7d3:	89 04 24             	mov    %eax,(%esp)
 804d7d6:	89 54 24 10          	mov    %edx,0x10(%esp)
 804d7da:	89 4c 24 14          	mov    %ecx,0x14(%esp)
 804d7de:	e8 e9 18 00 00       	call   804f0cc <_sbrk_r>
    if (new_brk == (char*)(MORECORE_FAILURE))
 804d7e3:	83 f8 ff             	cmp    $0xffffffff,%eax
 804d7e6:	8b 54 24 10          	mov    0x10(%esp),%edx
 804d7ea:	8b 4c 24 14          	mov    0x14(%esp),%ecx
 804d7ee:	0f 84 50 03 00 00    	je     804db44 <_malloc_r+0x688>
 804d7f4:	29 c8                	sub    %ecx,%eax
 804d7f6:	01 d0                	add    %edx,%eax
 804d7f8:	83 c8 01             	or     $0x1,%eax
	correction = 0;
	correction_failed = 1;
	new_brk = brk;
      }

    sbrked_mem += correction;
 804d7fb:	03 15 c0 49 05 08    	add    0x80549c0,%edx
 804d801:	89 15 c0 49 05 08    	mov    %edx,0x80549c0

    top = (mchunkptr)brk;
 804d807:	89 0d 68 45 05 08    	mov    %ecx,0x8054568
    top_size = new_brk - brk + correction;
    set_head(top, top_size | PREV_INUSE);
 804d80d:	89 41 04             	mov    %eax,0x4(%ecx)

    if (old_top != initial_top)
 804d810:	81 fe 60 45 05 08    	cmp    $0x8054560,%esi
 804d816:	74 33                	je     804d84b <_malloc_r+0x38f>

      /* There must have been an intervening foreign sbrk call. */
      /* A double fencepost is necessary to prevent consolidation */

      /* If not enough space to do this, then user did something very wrong */
      if (old_top_size < MINSIZE) 
 804d818:	83 ff 0f             	cmp    $0xf,%edi
 804d81b:	0f 86 88 02 00 00    	jbe    804daa9 <_malloc_r+0x5ed>
        set_head(top, PREV_INUSE); /* will force null return from malloc */
        return;
      }

      /* Also keep size a multiple of MALLOC_ALIGNMENT */
      old_top_size = (old_top_size - 3*SIZE_SZ) & ~MALLOC_ALIGN_MASK;
 804d821:	83 ef 0c             	sub    $0xc,%edi
 804d824:	83 e7 f8             	and    $0xfffffff8,%edi
      set_head_size(old_top, old_top_size);
 804d827:	8b 46 04             	mov    0x4(%esi),%eax
 804d82a:	83 e0 01             	and    $0x1,%eax
 804d82d:	09 f8                	or     %edi,%eax
 804d82f:	89 46 04             	mov    %eax,0x4(%esi)
      chunk_at_offset(old_top, old_top_size          )->size =
 804d832:	c7 44 3e 04 05 00 00 	movl   $0x5,0x4(%esi,%edi,1)
 804d839:	00 
        SIZE_SZ|PREV_INUSE;
      chunk_at_offset(old_top, old_top_size + SIZE_SZ)->size =
 804d83a:	c7 44 3e 08 05 00 00 	movl   $0x5,0x8(%esi,%edi,1)
 804d841:	00 
        SIZE_SZ|PREV_INUSE;
      /* If possible, release the rest. */
      if (old_top_size >= MINSIZE) 
 804d842:	83 ff 0f             	cmp    $0xf,%edi
 804d845:	0f 87 18 02 00 00    	ja     804da63 <_malloc_r+0x5a7>
 804d84b:	89 ce                	mov    %ecx,%esi
        fREe(RCALL chunk2mem(old_top));
    }
  }

  if ((unsigned long)sbrked_mem > (unsigned long)max_sbrked_mem) 
 804d84d:	3b 15 ec 49 05 08    	cmp    0x80549ec,%edx
 804d853:	76 06                	jbe    804d85b <_malloc_r+0x39f>
    max_sbrked_mem = sbrked_mem;
 804d855:	89 15 ec 49 05 08    	mov    %edx,0x80549ec
#if HAVE_MMAP
  if ((unsigned long)(mmapped_mem + sbrked_mem) > (unsigned long)max_total_mem) 
    max_total_mem = mmapped_mem + sbrked_mem;
#else
  if ((unsigned long)(sbrked_mem) > (unsigned long)max_total_mem) 
 804d85b:	3b 15 e8 49 05 08    	cmp    0x80549e8,%edx
 804d861:	76 06                	jbe    804d869 <_malloc_r+0x3ad>
    max_total_mem = sbrked_mem;
 804d863:	89 15 e8 49 05 08    	mov    %edx,0x80549e8
 804d869:	8b 56 04             	mov    0x4(%esi),%edx
 804d86c:	83 e2 fc             	and    $0xfffffffc,%edx
 804d86f:	e9 cb 01 00 00       	jmp    804da3f <_malloc_r+0x583>
      return 0; /* propagate failure */
    }
  }

  victim = top;
  set_head(victim, nb | PREV_INUSE);
 804d874:	89 da                	mov    %ebx,%edx
 804d876:	83 ca 01             	or     $0x1,%edx
 804d879:	89 56 04             	mov    %edx,0x4(%esi)
  top = chunk_at_offset(victim, nb);
 804d87c:	01 f3                	add    %esi,%ebx
 804d87e:	89 1d 68 45 05 08    	mov    %ebx,0x8054568
  set_head(top, remainder_size | PREV_INUSE);
 804d884:	83 c8 01             	or     $0x1,%eax
 804d887:	89 43 04             	mov    %eax,0x4(%ebx)
  check_malloced_chunk(victim, nb);
  MALLOC_UNLOCK;
 804d88a:	8b 44 24 40          	mov    0x40(%esp),%eax
 804d88e:	89 04 24             	mov    %eax,(%esp)
 804d891:	e8 16 05 00 00       	call   804ddac <__malloc_unlock>
  return chunk2mem(victim);
 804d896:	8d 46 08             	lea    0x8(%esi),%eax

#endif /* MALLOC_PROVIDED */
}
 804d899:	83 c4 2c             	add    $0x2c,%esp
 804d89c:	5b                   	pop    %ebx
 804d89d:	5e                   	pop    %esi
 804d89e:	5f                   	pop    %edi
 804d89f:	5d                   	pop    %ebp
 804d8a0:	c3                   	ret    
 804d8a1:	8d 76 00             	lea    0x0(%esi),%esi
    idx += 2; /* Set for bin scan below. We've already scanned 2 bins. */

  }
  else
  {
    idx = bin_index(nb);
 804d8a4:	83 ff 14             	cmp    $0x14,%edi
 804d8a7:	0f 86 5f 01 00 00    	jbe    804da0c <_malloc_r+0x550>
 804d8ad:	83 ff 54             	cmp    $0x54,%edi
 804d8b0:	0f 87 d1 01 00 00    	ja     804da87 <_malloc_r+0x5cb>
 804d8b6:	89 df                	mov    %ebx,%edi
 804d8b8:	c1 ef 0c             	shr    $0xc,%edi
 804d8bb:	83 c7 6e             	add    $0x6e,%edi
 804d8be:	8d 04 3f             	lea    (%edi,%edi,1),%eax
 804d8c1:	e9 ad fc ff ff       	jmp    804d573 <_malloc_r+0xb7>
 804d8c6:	66 90                	xchg   %ax,%ax
      return chunk2mem(victim);
    }

    /* Else place in bin */

    frontlink(victim, victim_size, remainder_index, bck, fwd);
 804d8c8:	89 f9                	mov    %edi,%ecx
 804d8ca:	c1 e9 09             	shr    $0x9,%ecx
 804d8cd:	83 f9 04             	cmp    $0x4,%ecx
 804d8d0:	0f 86 41 01 00 00    	jbe    804da17 <_malloc_r+0x55b>
 804d8d6:	83 f9 14             	cmp    $0x14,%ecx
 804d8d9:	0f 87 13 02 00 00    	ja     804daf2 <_malloc_r+0x636>
 804d8df:	83 c1 5b             	add    $0x5b,%ecx
 804d8e2:	8d 04 09             	lea    (%ecx,%ecx,1),%eax
 804d8e5:	8d 04 85 60 45 05 08 	lea    0x8054560(,%eax,4),%eax
 804d8ec:	8b 50 08             	mov    0x8(%eax),%edx
 804d8ef:	39 c2                	cmp    %eax,%edx
 804d8f1:	0f 84 bf 01 00 00    	je     804dab6 <_malloc_r+0x5fa>
 804d8f7:	90                   	nop
 804d8f8:	8b 4a 04             	mov    0x4(%edx),%ecx
 804d8fb:	83 e1 fc             	and    $0xfffffffc,%ecx
 804d8fe:	39 cf                	cmp    %ecx,%edi
 804d900:	73 07                	jae    804d909 <_malloc_r+0x44d>
 804d902:	8b 52 08             	mov    0x8(%edx),%edx
 804d905:	39 d0                	cmp    %edx,%eax
 804d907:	75 ef                	jne    804d8f8 <_malloc_r+0x43c>
 804d909:	8b 4a 0c             	mov    0xc(%edx),%ecx
 804d90c:	a1 64 45 05 08       	mov    0x8054564,%eax
 804d911:	89 4e 0c             	mov    %ecx,0xc(%esi)
 804d914:	89 56 08             	mov    %edx,0x8(%esi)
 804d917:	89 71 08             	mov    %esi,0x8(%ecx)
 804d91a:	89 72 0c             	mov    %esi,0xc(%edx)
 804d91d:	e9 fd fc ff ff       	jmp    804d61f <_malloc_r+0x163>
 804d922:	66 90                	xchg   %ax,%ax
    victim_size = chunksize(victim);
    remainder_size = long_sub_size_t(victim_size, nb);

    if (remainder_size >= (long)MINSIZE) /* re-split */
    {
      remainder = chunk_at_offset(victim, nb);
 804d924:	8d 14 1e             	lea    (%esi,%ebx,1),%edx
      set_head(victim, nb | PREV_INUSE);
 804d927:	83 cb 01             	or     $0x1,%ebx
 804d92a:	89 5e 04             	mov    %ebx,0x4(%esi)
      link_last_remainder(remainder);
 804d92d:	89 15 74 45 05 08    	mov    %edx,0x8054574
 804d933:	89 15 70 45 05 08    	mov    %edx,0x8054570
 804d939:	c7 42 0c 68 45 05 08 	movl   $0x8054568,0xc(%edx)
 804d940:	c7 42 08 68 45 05 08 	movl   $0x8054568,0x8(%edx)
      set_head(remainder, remainder_size | PREV_INUSE);
 804d947:	89 c1                	mov    %eax,%ecx
 804d949:	83 c9 01             	or     $0x1,%ecx
 804d94c:	89 4a 04             	mov    %ecx,0x4(%edx)
      set_foot(remainder, remainder_size);
 804d94f:	89 04 02             	mov    %eax,(%edx,%eax,1)
      check_malloced_chunk(victim, nb);
      MALLOC_UNLOCK;
 804d952:	8b 44 24 40          	mov    0x40(%esp),%eax
 804d956:	89 04 24             	mov    %eax,(%esp)
 804d959:	e8 4e 04 00 00       	call   804ddac <__malloc_unlock>
      return chunk2mem(victim);
 804d95e:	8d 46 08             	lea    0x8(%esi),%eax
 804d961:	e9 c3 fb ff ff       	jmp    804d529 <_malloc_r+0x6d>
 804d966:	66 90                	xchg   %ax,%ax
 804d968:	a1 64 45 05 08       	mov    0x8054564,%eax
 804d96d:	e9 ad fc ff ff       	jmp    804d61f <_malloc_r+0x163>
 804d972:	66 90                	xchg   %ax,%ax
            return chunk2mem(victim);
          }

        }

       bin = next_bin(bin);
 804d974:	83 c1 08             	add    $0x8,%ecx
         {
           bin = next_bin(bin);
           ++idx;
         }
#endif
      } while ((++idx & (BINBLOCKWIDTH - 1)) != 0);
 804d977:	ff 44 24 18          	incl   0x18(%esp)
 804d97b:	f6 44 24 18 03       	testb  $0x3,0x18(%esp)
 804d980:	0f 85 dc fc ff ff    	jne    804d662 <_malloc_r+0x1a6>
 804d986:	8b 44 24 1c          	mov    0x1c(%esp),%eax
 804d98a:	66 90                	xchg   %ax,%ax

      /* Clear out the block bit. */

      do   /* Possibly backtrack to try to clear a partial block */
      {
        if ((startidx & (BINBLOCKWIDTH - 1)) == 0)
 804d98c:	f7 c5 03 00 00 00    	test   $0x3,%ebp
 804d992:	0f 84 e8 01 00 00    	je     804db80 <_malloc_r+0x6c4>
        {
          binblocks &= ~block;
          break;
        }
        --startidx;
 804d998:	4d                   	dec    %ebp
       q = prev_bin(q);
 804d999:	8d 50 f8             	lea    -0x8(%eax),%edx
      } while (first(q) == q);
 804d99c:	8b 00                	mov    (%eax),%eax
 804d99e:	39 d0                	cmp    %edx,%eax
 804d9a0:	74 ea                	je     804d98c <_malloc_r+0x4d0>
 804d9a2:	a1 64 45 05 08       	mov    0x8054564,%eax

      /* Get to the next possibly nonempty block */

      if ( (block <<= 1) <= binblocks && (block != 0) ) 
 804d9a7:	d1 e7                	shl    %edi
 804d9a9:	39 c7                	cmp    %eax,%edi
 804d9ab:	0f 87 5b fd ff ff    	ja     804d70c <_malloc_r+0x250>
 804d9b1:	85 ff                	test   %edi,%edi
 804d9b3:	0f 84 53 fd ff ff    	je     804d70c <_malloc_r+0x250>
      {
        while ((block & binblocks) == 0)
 804d9b9:	85 f8                	test   %edi,%eax
 804d9bb:	0f 84 d3 01 00 00    	je     804db94 <_malloc_r+0x6d8>
      return chunk2mem(victim);
    }

    /* Else place in bin */

    frontlink(victim, victim_size, remainder_index, bck, fwd);
 804d9c1:	8b 6c 24 18          	mov    0x18(%esp),%ebp
 804d9c5:	e9 87 fc ff ff       	jmp    804d651 <_malloc_r+0x195>
 804d9ca:	66 90                	xchg   %ax,%ax

#if MALLOC_ALIGN != 16
    /* Also scan the next one, since it would have a remainder < MINSIZE */
    if (victim == q)
    {
      q = next_bin(q);
 804d9cc:	8d 46 08             	lea    0x8(%esi),%eax
      victim = last(q);
 804d9cf:	8b 76 14             	mov    0x14(%esi),%esi
    }
#endif
    if (victim != q)
 804d9d2:	39 f0                	cmp    %esi,%eax
 804d9d4:	0f 85 29 fb ff ff    	jne    804d503 <_malloc_r+0x47>
      check_malloced_chunk(victim, nb);
      MALLOC_UNLOCK;
      return chunk2mem(victim);
    }

    idx += 2; /* Set for bin scan below. We've already scanned 2 bins. */
 804d9da:	8d 6a 02             	lea    0x2(%edx),%ebp
 804d9dd:	e9 c0 fb ff ff       	jmp    804d5a2 <_malloc_r+0xe6>
 804d9e2:	66 90                	xchg   %ax,%ax
            return chunk2mem(victim);
          }

          else if (remainder_size >= 0)  /* take */
          {
            set_inuse_bit_at_offset(victim, victim_size);
 804d9e4:	83 4c 06 04 01       	orl    $0x1,0x4(%esi,%eax,1)
            unlink(victim, bck, fwd);
 804d9e9:	8b 46 0c             	mov    0xc(%esi),%eax
 804d9ec:	8b 56 08             	mov    0x8(%esi),%edx
 804d9ef:	89 42 0c             	mov    %eax,0xc(%edx)
 804d9f2:	89 50 08             	mov    %edx,0x8(%eax)
            check_malloced_chunk(victim, nb);
	    MALLOC_UNLOCK;
 804d9f5:	8b 44 24 40          	mov    0x40(%esp),%eax
 804d9f9:	89 04 24             	mov    %eax,(%esp)
 804d9fc:	e8 ab 03 00 00       	call   804ddac <__malloc_unlock>
            return chunk2mem(victim);
 804da01:	8d 46 08             	lea    0x8(%esi),%eax
 804da04:	e9 20 fb ff ff       	jmp    804d529 <_malloc_r+0x6d>
 804da09:	8d 76 00             	lea    0x0(%esi),%esi
    idx += 2; /* Set for bin scan below. We've already scanned 2 bins. */

  }
  else
  {
    idx = bin_index(nb);
 804da0c:	83 c7 5b             	add    $0x5b,%edi
 804da0f:	8d 04 3f             	lea    (%edi,%edi,1),%eax
 804da12:	e9 5c fb ff ff       	jmp    804d573 <_malloc_r+0xb7>
      return chunk2mem(victim);
    }

    /* Else place in bin */

    frontlink(victim, victim_size, remainder_index, bck, fwd);
 804da17:	89 f9                	mov    %edi,%ecx
 804da19:	c1 e9 06             	shr    $0x6,%ecx
 804da1c:	83 c1 38             	add    $0x38,%ecx
 804da1f:	8d 04 09             	lea    (%ecx,%ecx,1),%eax
 804da22:	e9 be fe ff ff       	jmp    804d8e5 <_malloc_r+0x429>

  brk = (char*)(MORECORE (sbrk_size));

  /* Fail if sbrk failed or if a foreign sbrk call killed our space */
  if (brk == (char*)(MORECORE_FAILURE) || 
      (brk < old_end && old_top != initial_top))
 804da27:	81 fe 60 45 05 08    	cmp    $0x8054560,%esi
 804da2d:	0f 84 43 fd ff ff    	je     804d776 <_malloc_r+0x2ba>
 804da33:	8b 35 68 45 05 08    	mov    0x8054568,%esi
 804da39:	8b 56 04             	mov    0x4(%esi),%edx
 804da3c:	83 e2 fc             	and    $0xfffffffc,%edx
    }
#endif

    /* Try to extend */
    malloc_extend_top(RCALL nb);
    remainder_size = long_sub_size_t(chunksize(top), nb);
 804da3f:	89 d0                	mov    %edx,%eax
 804da41:	29 d8                	sub    %ebx,%eax
    if (chunksize(top) < nb || remainder_size < (long)MINSIZE)
 804da43:	39 da                	cmp    %ebx,%edx
 804da45:	72 09                	jb     804da50 <_malloc_r+0x594>
 804da47:	83 f8 0f             	cmp    $0xf,%eax
 804da4a:	0f 8f 24 fe ff ff    	jg     804d874 <_malloc_r+0x3b8>
    {
      MALLOC_UNLOCK;
 804da50:	8b 44 24 40          	mov    0x40(%esp),%eax
 804da54:	89 04 24             	mov    %eax,(%esp)
 804da57:	e8 50 03 00 00       	call   804ddac <__malloc_unlock>
      return 0; /* propagate failure */
 804da5c:	31 c0                	xor    %eax,%eax
 804da5e:	e9 c6 fa ff ff       	jmp    804d529 <_malloc_r+0x6d>
        SIZE_SZ|PREV_INUSE;
      chunk_at_offset(old_top, old_top_size + SIZE_SZ)->size =
        SIZE_SZ|PREV_INUSE;
      /* If possible, release the rest. */
      if (old_top_size >= MINSIZE) 
        fREe(RCALL chunk2mem(old_top));
 804da63:	83 c6 08             	add    $0x8,%esi
 804da66:	89 74 24 04          	mov    %esi,0x4(%esp)
 804da6a:	8b 44 24 40          	mov    0x40(%esp),%eax
 804da6e:	89 04 24             	mov    %eax,(%esp)
 804da71:	e8 8a f0 ff ff       	call   804cb00 <_free_r>
 804da76:	8b 15 c0 49 05 08    	mov    0x80549c0,%edx
 804da7c:	8b 35 68 45 05 08    	mov    0x8054568,%esi
 804da82:	e9 c6 fd ff ff       	jmp    804d84d <_malloc_r+0x391>
    idx += 2; /* Set for bin scan below. We've already scanned 2 bins. */

  }
  else
  {
    idx = bin_index(nb);
 804da87:	81 ff 54 01 00 00    	cmp    $0x154,%edi
 804da8d:	77 43                	ja     804dad2 <_malloc_r+0x616>
 804da8f:	89 df                	mov    %ebx,%edi
 804da91:	c1 ef 0f             	shr    $0xf,%edi
 804da94:	83 c7 77             	add    $0x77,%edi
 804da97:	8d 04 3f             	lea    (%edi,%edi,1),%eax
 804da9a:	e9 d4 fa ff ff       	jmp    804d573 <_malloc_r+0xb7>
    else  /* Someone else called sbrk().  Count those bytes as sbrked_mem. */
      sbrked_mem += brk - (char*)old_end;

    /* Guarantee alignment of first new chunk made from this space */
    front_misalign = (POINTER_UINT)chunk2mem(brk) & MALLOC_ALIGN_MASK;
    if (front_misalign > 0) 
 804da9f:	ba 00 10 00 00       	mov    $0x1000,%edx
 804daa4:	e9 18 fd ff ff       	jmp    804d7c1 <_malloc_r+0x305>
      /* A double fencepost is necessary to prevent consolidation */

      /* If not enough space to do this, then user did something very wrong */
      if (old_top_size < MINSIZE) 
      {
        set_head(top, PREV_INUSE); /* will force null return from malloc */
 804daa9:	c7 41 04 01 00 00 00 	movl   $0x1,0x4(%ecx)
 804dab0:	89 ce                	mov    %ecx,%esi
 804dab2:	31 d2                	xor    %edx,%edx
 804dab4:	eb 89                	jmp    804da3f <_malloc_r+0x583>
      return chunk2mem(victim);
    }

    /* Else place in bin */

    frontlink(victim, victim_size, remainder_index, bck, fwd);
 804dab6:	c1 f9 02             	sar    $0x2,%ecx
 804dab9:	b8 01 00 00 00       	mov    $0x1,%eax
 804dabe:	d3 e0                	shl    %cl,%eax
 804dac0:	0b 05 64 45 05 08    	or     0x8054564,%eax
 804dac6:	a3 64 45 05 08       	mov    %eax,0x8054564
 804dacb:	89 d1                	mov    %edx,%ecx
 804dacd:	e9 3f fe ff ff       	jmp    804d911 <_malloc_r+0x455>
    idx += 2; /* Set for bin scan below. We've already scanned 2 bins. */

  }
  else
  {
    idx = bin_index(nb);
 804dad2:	81 ff 54 05 00 00    	cmp    $0x554,%edi
 804dad8:	77 50                	ja     804db2a <_malloc_r+0x66e>
 804dada:	89 df                	mov    %ebx,%edi
 804dadc:	c1 ef 12             	shr    $0x12,%edi
 804dadf:	83 c7 7c             	add    $0x7c,%edi
 804dae2:	8d 04 3f             	lea    (%edi,%edi,1),%eax
 804dae5:	e9 89 fa ff ff       	jmp    804d573 <_malloc_r+0xb7>
  INTERNAL_SIZE_T old_top_size = chunksize(old_top);
  char*     old_end      = (char*)(chunk_at_offset(old_top, old_top_size));

  /* Pad request with top_pad plus minimal overhead */
  
  INTERNAL_SIZE_T    sbrk_size     = nb + top_pad + MINSIZE;
 804daea:	8d 69 10             	lea    0x10(%ecx),%ebp
 804daed:	e9 5f fc ff ff       	jmp    804d751 <_malloc_r+0x295>
      return chunk2mem(victim);
    }

    /* Else place in bin */

    frontlink(victim, victim_size, remainder_index, bck, fwd);
 804daf2:	83 f9 54             	cmp    $0x54,%ecx
 804daf5:	77 59                	ja     804db50 <_malloc_r+0x694>
 804daf7:	89 f9                	mov    %edi,%ecx
 804daf9:	c1 e9 0c             	shr    $0xc,%ecx
 804dafc:	83 c1 6e             	add    $0x6e,%ecx
 804daff:	8d 04 09             	lea    (%ecx,%ecx,1),%eax
 804db02:	e9 de fd ff ff       	jmp    804d8e5 <_malloc_r+0x429>

  sbrked_mem += sbrk_size;

  if (brk == old_end /* can just add bytes to current top, unless
			previous correction failed */
      && ((POINTER_UINT)old_end & (pagesz - 1)) == 0)
 804db07:	f7 44 24 18 ff 0f 00 	testl  $0xfff,0x18(%esp)
 804db0e:	00 
 804db0f:	0f 85 79 fc ff ff    	jne    804d78e <_malloc_r+0x2d2>
  {
    top_size = sbrk_size + old_top_size;
    set_head(top, top_size | PREV_INUSE);
 804db15:	8b 35 68 45 05 08    	mov    0x8054568,%esi

  if (brk == old_end /* can just add bytes to current top, unless
			previous correction failed */
      && ((POINTER_UINT)old_end & (pagesz - 1)) == 0)
  {
    top_size = sbrk_size + old_top_size;
 804db1b:	8d 4c 3d 00          	lea    0x0(%ebp,%edi,1),%ecx
    set_head(top, top_size | PREV_INUSE);
 804db1f:	83 c9 01             	or     $0x1,%ecx
 804db22:	89 4e 04             	mov    %ecx,0x4(%esi)
 804db25:	e9 23 fd ff ff       	jmp    804d84d <_malloc_r+0x391>
    idx += 2; /* Set for bin scan below. We've already scanned 2 bins. */

  }
  else
  {
    idx = bin_index(nb);
 804db2a:	b8 fc 00 00 00       	mov    $0xfc,%eax
 804db2f:	bf 7e 00 00 00       	mov    $0x7e,%edi
 804db34:	e9 3a fa ff ff       	jmp    804d573 <_malloc_r+0xb7>
    set_head(top, top_size | PREV_INUSE);
  }
  else
  {
    if (sbrk_base == (char*)(-1))  /* First time through. Record base */
      sbrk_base = brk;
 804db39:	89 0d 40 45 05 08    	mov    %ecx,0x8054540
 804db3f:	e9 64 fc ff ff       	jmp    804d7a8 <_malloc_r+0x2ec>
    /* Guarantee the next brk will be at a page boundary */
    correction += pagesz - ((POINTER_UINT)(brk + sbrk_size) & (pagesz - 1));

    /* Allocate correction */
    new_brk = (char*)(MORECORE (correction));
    if (new_brk == (char*)(MORECORE_FAILURE))
 804db44:	b8 01 00 00 00       	mov    $0x1,%eax
      {
	correction = 0;
 804db49:	31 d2                	xor    %edx,%edx
 804db4b:	e9 ab fc ff ff       	jmp    804d7fb <_malloc_r+0x33f>
      return chunk2mem(victim);
    }

    /* Else place in bin */

    frontlink(victim, victim_size, remainder_index, bck, fwd);
 804db50:	81 f9 54 01 00 00    	cmp    $0x154,%ecx
 804db56:	77 10                	ja     804db68 <_malloc_r+0x6ac>
 804db58:	89 f9                	mov    %edi,%ecx
 804db5a:	c1 e9 0f             	shr    $0xf,%ecx
 804db5d:	83 c1 77             	add    $0x77,%ecx
 804db60:	8d 04 09             	lea    (%ecx,%ecx,1),%eax
 804db63:	e9 7d fd ff ff       	jmp    804d8e5 <_malloc_r+0x429>
 804db68:	81 f9 54 05 00 00    	cmp    $0x554,%ecx
 804db6e:	77 30                	ja     804dba0 <_malloc_r+0x6e4>
 804db70:	89 f9                	mov    %edi,%ecx
 804db72:	c1 e9 12             	shr    $0x12,%ecx
 804db75:	83 c1 7c             	add    $0x7c,%ecx
 804db78:	8d 04 09             	lea    (%ecx,%ecx,1),%eax
 804db7b:	e9 65 fd ff ff       	jmp    804d8e5 <_malloc_r+0x429>

      do   /* Possibly backtrack to try to clear a partial block */
      {
        if ((startidx & (BINBLOCKWIDTH - 1)) == 0)
        {
          binblocks &= ~block;
 804db80:	89 f8                	mov    %edi,%eax
 804db82:	f7 d0                	not    %eax
 804db84:	23 05 64 45 05 08    	and    0x8054564,%eax
 804db8a:	a3 64 45 05 08       	mov    %eax,0x8054564
          break;
 804db8f:	e9 13 fe ff ff       	jmp    804d9a7 <_malloc_r+0x4eb>

      if ( (block <<= 1) <= binblocks && (block != 0) ) 
      {
        while ((block & binblocks) == 0)
        {
          idx += BINBLOCKWIDTH;
 804db94:	83 44 24 18 04       	addl   $0x4,0x18(%esp)
          block <<= 1;
 804db99:	d1 e7                	shl    %edi
 804db9b:	e9 19 fe ff ff       	jmp    804d9b9 <_malloc_r+0x4fd>
      return chunk2mem(victim);
    }

    /* Else place in bin */

    frontlink(victim, victim_size, remainder_index, bck, fwd);
 804dba0:	b8 fc 00 00 00       	mov    $0xfc,%eax
 804dba5:	b9 7e 00 00 00       	mov    $0x7e,%ecx
 804dbaa:	e9 36 fd ff ff       	jmp    804d8e5 <_malloc_r+0x429>
 804dbaf:	90                   	nop

0804dbb0 <memchr>:

	.global	SYM (memchr)
       SOTYPE_FUNCTION(memchr)

SYM (memchr):
	pushl	ebp
 804dbb0:	55                   	push   %ebp
	movl	esp,ebp
 804dbb1:	89 e5                	mov    %esp,%ebp
	pushl	edi
 804dbb3:	57                   	push   %edi
	movzbl	12(ebp),eax
 804dbb4:	0f b6 45 0c          	movzbl 0xc(%ebp),%eax
	movl	16(ebp),ecx
 804dbb8:	8b 4d 10             	mov    0x10(%ebp),%ecx
	movl	8(ebp),edi
 804dbbb:	8b 7d 08             	mov    0x8(%ebp),%edi

	xorl	edx,edx
 804dbbe:	31 d2                	xor    %edx,%edx
	testl	ecx,ecx
 804dbc0:	85 c9                	test   %ecx,%ecx
	jz	L20
 804dbc2:	74 79                	je     804dc3d <L20>
	setnz	dl
	decl	edi

#else /* !__OPTIMIZE_SIZE__ */
/* Do byte-wise checks until string is aligned.  */
	testl	$3,edi
 804dbc4:	f7 c7 03 00 00 00    	test   $0x3,%edi
	je	L5
 804dbca:	74 28                	je     804dbf4 <L5>
	cmpb	(edi),al
 804dbcc:	3a 07                	cmp    (%edi),%al
	je	L15
 804dbce:	74 6a                	je     804dc3a <L15>
	incl	edi
 804dbd0:	47                   	inc    %edi
	decl	ecx
 804dbd1:	49                   	dec    %ecx
	je	L20
 804dbd2:	74 69                	je     804dc3d <L20>

	testl	$3,edi
 804dbd4:	f7 c7 03 00 00 00    	test   $0x3,%edi
	je	L5
 804dbda:	74 18                	je     804dbf4 <L5>
	cmpb	(edi),al
 804dbdc:	3a 07                	cmp    (%edi),%al
	je	L15
 804dbde:	74 5a                	je     804dc3a <L15>
	incl	edi
 804dbe0:	47                   	inc    %edi
	decl	ecx
 804dbe1:	49                   	dec    %ecx
	je	L20
 804dbe2:	74 59                	je     804dc3d <L20>

	testl	$3,edi
 804dbe4:	f7 c7 03 00 00 00    	test   $0x3,%edi
	je	L5
 804dbea:	74 08                	je     804dbf4 <L5>
	cmpb	(edi),al
 804dbec:	3a 07                	cmp    (%edi),%al
	je	L15
 804dbee:	74 4a                	je     804dc3a <L15>
	incl	edi
 804dbf0:	47                   	inc    %edi
	decl	ecx
 804dbf1:	49                   	dec    %ecx
	je	L20
 804dbf2:	74 49                	je     804dc3d <L20>

0804dbf4 <L5>:

/* Create a mask, then check a word at a time.  */
L5:
	movb	al,ah
 804dbf4:	88 c4                	mov    %al,%ah
	movl	eax,edx
 804dbf6:	89 c2                	mov    %eax,%edx
	sall	$16,edx
 804dbf8:	c1 e2 10             	shl    $0x10,%edx
	orl	edx,eax
 804dbfb:	09 d0                	or     %edx,%eax
	pushl	ebx
 804dbfd:	53                   	push   %ebx
 804dbfe:	66 90                	xchg   %ax,%ax

0804dc00 <L8>:

	.p2align 4,,7
L8:
	subl	$4,ecx
 804dc00:	83 e9 04             	sub    $0x4,%ecx
	jc	L9
 804dc03:	72 1c                	jb     804dc21 <L9>
	movl	(edi),edx
 804dc05:	8b 17                	mov    (%edi),%edx
	addl	$4,edi
 804dc07:	83 c7 04             	add    $0x4,%edi
	xorl	eax,edx
 804dc0a:	31 c2                	xor    %eax,%edx
	leal	-16843009(edx),ebx
 804dc0c:	8d 9a ff fe fe fe    	lea    -0x1010101(%edx),%ebx
	notl	edx
 804dc12:	f7 d2                	not    %edx
	andl	edx,ebx
 804dc14:	21 d3                	and    %edx,%ebx
	testl	$-2139062144,ebx
 804dc16:	f7 c3 80 80 80 80    	test   $0x80808080,%ebx
	je	L8
 804dc1c:	74 e2                	je     804dc00 <L8>

	subl	$4,edi
 804dc1e:	83 ef 04             	sub    $0x4,%edi

0804dc21 <L9>:

L9:
	popl	ebx
 804dc21:	5b                   	pop    %ebx
	xorl	edx,edx
 804dc22:	31 d2                	xor    %edx,%edx
	addl	$4,ecx
 804dc24:	83 c1 04             	add    $0x4,%ecx
	je	L20
 804dc27:	74 14                	je     804dc3d <L20>
 804dc29:	8d b4 26 00 00 00 00 	lea    0x0(%esi,%eiz,1),%esi

0804dc30 <L10>:

/* Final byte-wise checks.  */
	.p2align 4,,7
L10:
	cmpb	(edi),al
 804dc30:	3a 07                	cmp    (%edi),%al
	je	L15
 804dc32:	74 06                	je     804dc3a <L15>
	incl	edi
 804dc34:	47                   	inc    %edi
	decl	ecx
 804dc35:	49                   	dec    %ecx
	jne	L10
 804dc36:	75 f8                	jne    804dc30 <L10>

	xorl	edi,edi
 804dc38:	31 ff                	xor    %edi,%edi

0804dc3a <L15>:

#endif /* !__OPTIMIZE_SIZE__ */

L15:
	decl	edx
 804dc3a:	4a                   	dec    %edx
	andl	edi,edx
 804dc3b:	21 fa                	and    %edi,%edx

0804dc3d <L20>:
L20:
	movl	edx,eax
 804dc3d:	89 d0                	mov    %edx,%eax

	leal	-4(ebp),esp
 804dc3f:	8d 65 fc             	lea    -0x4(%ebp),%esp
	popl	edi
 804dc42:	5f                   	pop    %edi
	leave
 804dc43:	c9                   	leave  
	ret
 804dc44:	c3                   	ret    
 804dc45:	90                   	nop
 804dc46:	90                   	nop
 804dc47:	90                   	nop

0804dc48 <memcpy>:
	.global SYM (memcpy)
       SOTYPE_FUNCTION(memcpy)

SYM (memcpy):

	pushl ebp
 804dc48:	55                   	push   %ebp
	movl esp,ebp
 804dc49:	89 e5                	mov    %esp,%ebp
	pushl esi
 804dc4b:	56                   	push   %esi
	pushl edi
 804dc4c:	57                   	push   %edi
	pushl ebx
 804dc4d:	53                   	push   %ebx
	movl 8(ebp),edi
 804dc4e:	8b 7d 08             	mov    0x8(%ebp),%edi
	movl 16(ebp),ecx
 804dc51:	8b 4d 10             	mov    0x10(%ebp),%ecx
	movl 12(ebp),esi
 804dc54:	8b 75 0c             	mov    0xc(%ebp),%esi
	cld
 804dc57:	fc                   	cld    
		
#ifndef __OPTIMIZE_SIZE__
		
	cmpl $8,ecx
 804dc58:	83 f9 08             	cmp    $0x8,%ecx
	jbe .L3
 804dc5b:	76 26                	jbe    804dc83 <memcpy+0x3b>

/* move any preceding bytes until destination address is long word aligned */
		
	movl edi,edx	
 804dc5d:	89 fa                	mov    %edi,%edx
	movl ecx,ebx
 804dc5f:	89 cb                	mov    %ecx,%ebx
	andl $3,edx
 804dc61:	83 e2 03             	and    $0x3,%edx
	jz .L11
 804dc64:	74 10                	je     804dc76 <memcpy+0x2e>
	movl $4,ecx
 804dc66:	b9 04 00 00 00       	mov    $0x4,%ecx
	subl edx,ecx
 804dc6b:	29 d1                	sub    %edx,%ecx
	andl $3,ecx
 804dc6d:	83 e1 03             	and    $0x3,%ecx
	subl ecx,ebx
 804dc70:	29 cb                	sub    %ecx,%ebx
	rep
 804dc72:	f3 a4                	rep movsb %ds:(%esi),%es:(%edi)
	movsb
	
	mov ebx,ecx
 804dc74:	89 d9                	mov    %ebx,%ecx

/* move bytes a long word at a time */
		
.L11:
	shrl $2,ecx
 804dc76:	c1 e9 02             	shr    $0x2,%ecx
 804dc79:	8d 76 00             	lea    0x0(%esi),%esi
	.p2align 2
	rep
 804dc7c:	f3 a5                	rep movsl %ds:(%esi),%es:(%edi)
	movsl
	
	movl ebx,ecx
 804dc7e:	89 d9                	mov    %ebx,%ecx
	andl $3,ecx
 804dc80:	83 e1 03             	and    $0x3,%ecx
#endif /* !__OPTIMIZE_SIZE__ */

/* handle any remaining bytes */
		
.L3:		
	rep
 804dc83:	f3 a4                	rep movsb %ds:(%esi),%es:(%edi)
	movsb
.L5:		
	movl 8(ebp),eax
 804dc85:	8b 45 08             	mov    0x8(%ebp),%eax

	leal -12(ebp),esp
 804dc88:	8d 65 f4             	lea    -0xc(%ebp),%esp
	popl ebx
 804dc8b:	5b                   	pop    %ebx
	popl edi
 804dc8c:	5f                   	pop    %edi
	popl esi
 804dc8d:	5e                   	pop    %esi
	leave
 804dc8e:	c9                   	leave  
	ret
 804dc8f:	c3                   	ret    

0804dc90 <memmove>:
	.global SYM (memmove)
       SOTYPE_FUNCTION(memmove)

SYM (memmove):

	pushl ebp
 804dc90:	55                   	push   %ebp
	movl esp,ebp
 804dc91:	89 e5                	mov    %esp,%ebp
	pushl esi
 804dc93:	56                   	push   %esi
	pushl edi
 804dc94:	57                   	push   %edi
	pushl ebx
 804dc95:	53                   	push   %ebx
	movl 8(ebp),edi
 804dc96:	8b 7d 08             	mov    0x8(%ebp),%edi
	movl 16(ebp),ecx
 804dc99:	8b 4d 10             	mov    0x10(%ebp),%ecx
	movl 12(ebp),esi
 804dc9c:	8b 75 0c             	mov    0xc(%ebp),%esi

/*  check for destructive overlap (src < dst && dst < src + length) */

	cld
 804dc9f:	fc                   	cld    
	cmpl edi,esi
 804dca0:	39 fe                	cmp    %edi,%esi
	jae  .L2
 804dca2:	73 43                	jae    804dce7 <memmove+0x57>
	leal -1(ecx,esi),ebx
 804dca4:	8d 5c 31 ff          	lea    -0x1(%ecx,%esi,1),%ebx
	cmpl ebx,edi
 804dca8:	39 df                	cmp    %ebx,%edi
	ja   .L2
 804dcaa:	77 3b                	ja     804dce7 <memmove+0x57>

/* IF:	 destructive overlap, must copy backwards */

	addl ecx,esi
 804dcac:	01 ce                	add    %ecx,%esi
	addl ecx,edi
 804dcae:	01 cf                	add    %ecx,%edi
	std
 804dcb0:	fd                   	std    

#ifndef __OPTIMIZE_SIZE__
	
	cmpl $8,ecx
 804dcb1:	83 f9 08             	cmp    $0x8,%ecx
	jbe .L13
 804dcb4:	76 2b                	jbe    804dce1 <memmove+0x51>
.L18:

/* move trailing bytes in reverse until destination address is long word aligned */
		
	movl edi,edx
 804dcb6:	89 fa                	mov    %edi,%edx
	movl ecx,ebx
 804dcb8:	89 cb                	mov    %ecx,%ebx
	andl $3,edx
 804dcba:	83 e2 03             	and    $0x3,%edx
	jz .L21
 804dcbd:	74 0c                	je     804dccb <memmove+0x3b>
	
	movl edx,ecx
 804dcbf:	89 d1                	mov    %edx,%ecx
	decl esi
 804dcc1:	4e                   	dec    %esi
	decl edi
 804dcc2:	4f                   	dec    %edi
	subl ecx,ebx
 804dcc3:	29 cb                	sub    %ecx,%ebx
	rep
 804dcc5:	f3 a4                	rep movsb %ds:(%esi),%es:(%edi)
	movsb
	
	mov ebx,ecx
 804dcc7:	89 d9                	mov    %ebx,%ecx
	incl esi
 804dcc9:	46                   	inc    %esi
	incl edi
 804dcca:	47                   	inc    %edi

.L21:
	
/* move bytes in reverse, a long word at a time */
		
	shrl $2,ecx
 804dccb:	c1 e9 02             	shr    $0x2,%ecx
	subl $4,esi
 804dcce:	83 ee 04             	sub    $0x4,%esi
	subl $4,edi
 804dcd1:	83 ef 04             	sub    $0x4,%edi
	rep
 804dcd4:	f3 a5                	rep movsl %ds:(%esi),%es:(%edi)
	movsl
	
	addl $4,esi
 804dcd6:	83 c6 04             	add    $0x4,%esi
	addl $4,edi
 804dcd9:	83 c7 04             	add    $0x4,%edi
	movl ebx,ecx
 804dcdc:	89 d9                	mov    %ebx,%ecx
	andl $3,ecx
 804dcde:	83 e1 03             	and    $0x3,%ecx
#endif /* !__OPTIMIZE_SIZE__ */

/* handle any remaining bytes not on a long word boundary */			
	
.L13: 
	decl esi
 804dce1:	4e                   	dec    %esi
	decl edi
 804dce2:	4f                   	dec    %edi

.L15:
	rep
 804dce3:	f3 a4                	rep movsb %ds:(%esi),%es:(%edi)
	movsb
	jmp .L5
 804dce5:	eb 2a                	jmp    804dd11 <memmove+0x81>
		
.L2:

#ifndef __OPTIMIZE_SIZE__
		
	cmpl $8,ecx
 804dce7:	83 f9 08             	cmp    $0x8,%ecx
	jbe .L3
 804dcea:	76 23                	jbe    804dd0f <memmove+0x7f>

/* move any preceding bytes until destination address is long word aligned */
		
	movl edi,edx	
 804dcec:	89 fa                	mov    %edi,%edx
	movl ecx,ebx
 804dcee:	89 cb                	mov    %ecx,%ebx
	andl $3,edx
 804dcf0:	83 e2 03             	and    $0x3,%edx
	jz .L11
 804dcf3:	74 10                	je     804dd05 <memmove+0x75>
	movl $4,ecx
 804dcf5:	b9 04 00 00 00       	mov    $0x4,%ecx
	subl edx,ecx
 804dcfa:	29 d1                	sub    %edx,%ecx
	andl $3,ecx
 804dcfc:	83 e1 03             	and    $0x3,%ecx
	subl ecx,ebx
 804dcff:	29 cb                	sub    %ecx,%ebx
	rep
 804dd01:	f3 a4                	rep movsb %ds:(%esi),%es:(%edi)
	movsb
	
	mov ebx,ecx
 804dd03:	89 d9                	mov    %ebx,%ecx

/* move bytes a long word at a time */
		
.L11:
	shrl $2,ecx
 804dd05:	c1 e9 02             	shr    $0x2,%ecx
	.p2align 2
	rep
 804dd08:	f3 a5                	rep movsl %ds:(%esi),%es:(%edi)
	movsl
	
	movl ebx,ecx
 804dd0a:	89 d9                	mov    %ebx,%ecx
	andl $3,ecx
 804dd0c:	83 e1 03             	and    $0x3,%ecx
#endif /* !__OPTIMIZE_SIZE__ */

/* handle any remaining bytes */
		
.L3:		
	rep
 804dd0f:	f3 a4                	rep movsb %ds:(%esi),%es:(%edi)
	movsb
.L5:		
	movl 8(ebp),eax
 804dd11:	8b 45 08             	mov    0x8(%ebp),%eax
	cld
 804dd14:	fc                   	cld    

	leal -12(ebp),esp
 804dd15:	8d 65 f4             	lea    -0xc(%ebp),%esp
	popl ebx
 804dd18:	5b                   	pop    %ebx
	popl edi
 804dd19:	5f                   	pop    %edi
	popl esi
 804dd1a:	5e                   	pop    %esi
	leave
 804dd1b:	c9                   	leave  
	ret
 804dd1c:	c3                   	ret    
 804dd1d:	90                   	nop
 804dd1e:	90                   	nop
 804dd1f:	90                   	nop

0804dd20 <memset>:
	.global SYM (memset)
       SOTYPE_FUNCTION(memset)

SYM (memset):

	pushl ebp
 804dd20:	55                   	push   %ebp
	movl esp,ebp
 804dd21:	89 e5                	mov    %esp,%ebp
	pushl edi
 804dd23:	57                   	push   %edi
	movl 8(ebp),edi
 804dd24:	8b 7d 08             	mov    0x8(%ebp),%edi
	movzbl 12(ebp),eax
 804dd27:	0f b6 45 0c          	movzbl 0xc(%ebp),%eax
	movl 16(ebp),ecx
 804dd2b:	8b 4d 10             	mov    0x10(%ebp),%ecx
	cld
 804dd2e:	fc                   	cld    

#ifndef __OPTIMIZE_SIZE__
/* Less than 16 bytes won't benefit from the 'rep stosl' loop.  */
	cmpl $16,ecx
 804dd2f:	83 f9 10             	cmp    $0x10,%ecx
	jbe .L19
 804dd32:	76 69                	jbe    804dd9d <memset+0x7d>
	testl $7,edi
 804dd34:	f7 c7 07 00 00 00    	test   $0x7,%edi
	je .L10
 804dd3a:	74 4c                	je     804dd88 <memset+0x68>

/* It turns out that 8-byte aligned 'rep stosl' outperforms
   4-byte aligned on some x86 platforms.  */
	movb al,(edi)
 804dd3c:	88 07                	mov    %al,(%edi)
	incl edi
 804dd3e:	47                   	inc    %edi
	decl ecx
 804dd3f:	49                   	dec    %ecx
	testl $7,edi
 804dd40:	f7 c7 07 00 00 00    	test   $0x7,%edi
	je .L10
 804dd46:	74 40                	je     804dd88 <memset+0x68>

	movb al,(edi)
 804dd48:	88 07                	mov    %al,(%edi)
	incl edi
 804dd4a:	47                   	inc    %edi
	decl ecx
 804dd4b:	49                   	dec    %ecx
	testl $7,edi
 804dd4c:	f7 c7 07 00 00 00    	test   $0x7,%edi
	je .L10
 804dd52:	74 34                	je     804dd88 <memset+0x68>

	movb al,(edi)
 804dd54:	88 07                	mov    %al,(%edi)
	incl edi
 804dd56:	47                   	inc    %edi
	decl ecx
 804dd57:	49                   	dec    %ecx
	testl $7,edi
 804dd58:	f7 c7 07 00 00 00    	test   $0x7,%edi
	je .L10
 804dd5e:	74 28                	je     804dd88 <memset+0x68>

	movb al,(edi)
 804dd60:	88 07                	mov    %al,(%edi)
	incl edi
 804dd62:	47                   	inc    %edi
	decl ecx
 804dd63:	49                   	dec    %ecx
	testl $7,edi
 804dd64:	f7 c7 07 00 00 00    	test   $0x7,%edi
	je .L10
 804dd6a:	74 1c                	je     804dd88 <memset+0x68>

	movb al,(edi)
 804dd6c:	88 07                	mov    %al,(%edi)
	incl edi
 804dd6e:	47                   	inc    %edi
	decl ecx
 804dd6f:	49                   	dec    %ecx
	testl $7,edi
 804dd70:	f7 c7 07 00 00 00    	test   $0x7,%edi
	je .L10
 804dd76:	74 10                	je     804dd88 <memset+0x68>

	movb al,(edi)
 804dd78:	88 07                	mov    %al,(%edi)
	incl edi
 804dd7a:	47                   	inc    %edi
	decl ecx
 804dd7b:	49                   	dec    %ecx
	testl $7,edi
 804dd7c:	f7 c7 07 00 00 00    	test   $0x7,%edi
	je .L10
 804dd82:	74 04                	je     804dd88 <memset+0x68>

	movb al,(edi)
 804dd84:	88 07                	mov    %al,(%edi)
	incl edi
 804dd86:	47                   	inc    %edi
	decl ecx
 804dd87:	49                   	dec    %ecx

/* At this point, ecx>8 and edi%8==0.  */
.L10:
	movb al,ah
 804dd88:	88 c4                	mov    %al,%ah
	movl eax,edx
 804dd8a:	89 c2                	mov    %eax,%edx
	sall $16,edx
 804dd8c:	c1 e2 10             	shl    $0x10,%edx
	orl edx,eax
 804dd8f:	09 d0                	or     %edx,%eax

	movl ecx,edx
 804dd91:	89 ca                	mov    %ecx,%edx
	shrl $2,ecx
 804dd93:	c1 e9 02             	shr    $0x2,%ecx
	andl $3,edx
 804dd96:	83 e2 03             	and    $0x3,%edx
	rep
 804dd99:	f3 ab                	rep stos %eax,%es:(%edi)
	stosl
	movl edx,ecx
 804dd9b:	89 d1                	mov    %edx,%ecx
#endif /* not __OPTIMIZE_SIZE__ */

.L19:
	rep
 804dd9d:	f3 aa                	rep stos %al,%es:(%edi)
	stosb

	movl 8(ebp),eax
 804dd9f:	8b 45 08             	mov    0x8(%ebp),%eax

	leal -4(ebp),esp
 804dda2:	8d 65 fc             	lea    -0x4(%ebp),%esp
	popl edi
 804dda5:	5f                   	pop    %edi
	leave
 804dda6:	c9                   	leave  
	ret
 804dda7:	c3                   	ret    

0804dda8 <__malloc_lock>:
#endif

void
__malloc_lock (ptr)
     struct _reent *ptr;
{
 804dda8:	c3                   	ret    
 804dda9:	8d 76 00             	lea    0x0(%esi),%esi

0804ddac <__malloc_unlock>:
}

void
__malloc_unlock (ptr)
     struct _reent *ptr;
{
 804ddac:	c3                   	ret    
 804ddad:	90                   	nop
 804ddae:	90                   	nop
 804ddaf:	90                   	nop

0804ddb0 <_Balloc>:
#define _Kmax 15
*/

_Bigint *
_DEFUN (Balloc, (ptr, k), struct _reent *ptr _AND int k)
{
 804ddb0:	57                   	push   %edi
 804ddb1:	56                   	push   %esi
 804ddb2:	53                   	push   %ebx
 804ddb3:	83 ec 10             	sub    $0x10,%esp
 804ddb6:	8b 74 24 20          	mov    0x20(%esp),%esi
 804ddba:	8b 5c 24 24          	mov    0x24(%esp),%ebx
  int x;
  _Bigint *rv ;

  _REENT_CHECK_MP(ptr);
  if (_REENT_MP_FREELIST(ptr) == NULL)
 804ddbe:	8b 46 4c             	mov    0x4c(%esi),%eax
 804ddc1:	85 c0                	test   %eax,%eax
 804ddc3:	74 23                	je     804dde8 <_Balloc+0x38>
	{
	  return NULL;
	}
    }

  if ((rv = _REENT_MP_FREELIST(ptr)[k]) != 0)
 804ddc5:	8d 14 98             	lea    (%eax,%ebx,4),%edx
 804ddc8:	8b 02                	mov    (%edx),%eax
 804ddca:	85 c0                	test   %eax,%eax
 804ddcc:	74 3e                	je     804de0c <_Balloc+0x5c>
    {
      _REENT_MP_FREELIST(ptr)[k] = rv->_next;
 804ddce:	8b 08                	mov    (%eax),%ecx
 804ddd0:	89 0a                	mov    %ecx,(%edx)
				  (x-1) * sizeof(rv->_x));
      if (rv == NULL) return NULL;
      rv->_k = k;
      rv->_maxwds = x;
    }
  rv->_sign = rv->_wds = 0;
 804ddd2:	c7 40 10 00 00 00 00 	movl   $0x0,0x10(%eax)
 804ddd9:	c7 40 0c 00 00 00 00 	movl   $0x0,0xc(%eax)
  return rv;
}
 804dde0:	83 c4 10             	add    $0x10,%esp
 804dde3:	5b                   	pop    %ebx
 804dde4:	5e                   	pop    %esi
 804dde5:	5f                   	pop    %edi
 804dde6:	c3                   	ret    
 804dde7:	90                   	nop

  _REENT_CHECK_MP(ptr);
  if (_REENT_MP_FREELIST(ptr) == NULL)
    {
      /* Allocate a list of pointers to the mprec objects */
      _REENT_MP_FREELIST(ptr) = (struct _Bigint **) _calloc_r (ptr, 
 804dde8:	c7 44 24 08 21 00 00 	movl   $0x21,0x8(%esp)
 804ddef:	00 
 804ddf0:	c7 44 24 04 04 00 00 	movl   $0x4,0x4(%esp)
 804ddf7:	00 
 804ddf8:	89 34 24             	mov    %esi,(%esp)
 804ddfb:	e8 04 25 00 00       	call   8050304 <_calloc_r>
 804de00:	89 46 4c             	mov    %eax,0x4c(%esi)
						      sizeof (struct _Bigint *),
						      _Kmax + 1);
      if (_REENT_MP_FREELIST(ptr) == NULL)
 804de03:	85 c0                	test   %eax,%eax
 804de05:	75 be                	jne    804ddc5 <_Balloc+0x15>
	{
	  return NULL;
 804de07:	31 c0                	xor    %eax,%eax
 804de09:	eb d5                	jmp    804dde0 <_Balloc+0x30>
 804de0b:	90                   	nop
    {
      _REENT_MP_FREELIST(ptr)[k] = rv->_next;
    }
  else
    {
      x = 1 << k;
 804de0c:	bf 01 00 00 00       	mov    $0x1,%edi
 804de11:	88 d9                	mov    %bl,%cl
 804de13:	d3 e7                	shl    %cl,%edi
      /* Allocate an mprec Bigint and stick in in the freelist */
      rv = (_Bigint *) _calloc_r (ptr,
 804de15:	8d 04 bd 14 00 00 00 	lea    0x14(,%edi,4),%eax
 804de1c:	89 44 24 08          	mov    %eax,0x8(%esp)
 804de20:	c7 44 24 04 01 00 00 	movl   $0x1,0x4(%esp)
 804de27:	00 
 804de28:	89 34 24             	mov    %esi,(%esp)
 804de2b:	e8 d4 24 00 00       	call   8050304 <_calloc_r>
				  1,
				  sizeof (_Bigint) +
				  (x-1) * sizeof(rv->_x));
      if (rv == NULL) return NULL;
 804de30:	85 c0                	test   %eax,%eax
 804de32:	74 d3                	je     804de07 <_Balloc+0x57>
      rv->_k = k;
 804de34:	89 58 04             	mov    %ebx,0x4(%eax)
      rv->_maxwds = x;
 804de37:	89 78 08             	mov    %edi,0x8(%eax)
 804de3a:	eb 96                	jmp    804ddd2 <_Balloc+0x22>

0804de3c <_Bfree>:
  return rv;
}

void
_DEFUN (Bfree, (ptr, v), struct _reent *ptr _AND _Bigint * v)
{
 804de3c:	8b 44 24 08          	mov    0x8(%esp),%eax
  _REENT_CHECK_MP(ptr);
  if (v)
 804de40:	85 c0                	test   %eax,%eax
 804de42:	74 13                	je     804de57 <_Bfree+0x1b>
    {
      v->_next = _REENT_MP_FREELIST(ptr)[v->_k];
 804de44:	8b 48 04             	mov    0x4(%eax),%ecx
 804de47:	8b 54 24 04          	mov    0x4(%esp),%edx
 804de4b:	8b 52 4c             	mov    0x4c(%edx),%edx
 804de4e:	8d 14 8a             	lea    (%edx,%ecx,4),%edx
 804de51:	8b 0a                	mov    (%edx),%ecx
 804de53:	89 08                	mov    %ecx,(%eax)
      _REENT_MP_FREELIST(ptr)[v->_k] = v;
 804de55:	89 02                	mov    %eax,(%edx)
 804de57:	c3                   	ret    

0804de58 <__multadd>:
_DEFUN (multadd, (ptr, b, m, a),
	struct _reent *ptr _AND
	_Bigint * b _AND
	int m _AND
	int a)
{
 804de58:	55                   	push   %ebp
 804de59:	57                   	push   %edi
 804de5a:	56                   	push   %esi
 804de5b:	53                   	push   %ebx
 804de5c:	83 ec 2c             	sub    $0x2c,%esp
 804de5f:	8b 6c 24 48          	mov    0x48(%esp),%ebp
 804de63:	8b 5c 24 4c          	mov    0x4c(%esp),%ebx
#ifdef Pack_32
  __ULong xi, z;
#endif
  _Bigint *b1;

  wds = b->_wds;
 804de67:	8b 44 24 44          	mov    0x44(%esp),%eax
 804de6b:	8b 40 10             	mov    0x10(%eax),%eax
 804de6e:	89 44 24 1c          	mov    %eax,0x1c(%esp)
  x = b->_x;
 804de72:	8b 74 24 44          	mov    0x44(%esp),%esi
 804de76:	83 c6 14             	add    $0x14,%esi
  i = 0;
 804de79:	31 c0                	xor    %eax,%eax
 804de7b:	90                   	nop
  do
    {
#ifdef Pack_32
      xi = *x;
 804de7c:	8b 14 86             	mov    (%esi,%eax,4),%edx
      y = (xi & 0xffff) * m + a;
 804de7f:	0f b7 ca             	movzwl %dx,%ecx
 804de82:	0f af cd             	imul   %ebp,%ecx
 804de85:	01 d9                	add    %ebx,%ecx
      z = (xi >> 16) * m + (y >> 16);
 804de87:	89 cb                	mov    %ecx,%ebx
 804de89:	c1 eb 10             	shr    $0x10,%ebx
 804de8c:	c1 ea 10             	shr    $0x10,%edx
 804de8f:	0f af d5             	imul   %ebp,%edx
 804de92:	01 da                	add    %ebx,%edx
      a = (int) (z >> 16);
 804de94:	89 d7                	mov    %edx,%edi
 804de96:	c1 ef 10             	shr    $0x10,%edi
 804de99:	89 fb                	mov    %edi,%ebx
      *x++ = (z << 16) + (y & 0xffff);
 804de9b:	c1 e2 10             	shl    $0x10,%edx
 804de9e:	81 e1 ff ff 00 00    	and    $0xffff,%ecx
 804dea4:	01 ca                	add    %ecx,%edx
 804dea6:	89 14 86             	mov    %edx,(%esi,%eax,4)
      y = *x * m + a;
      a = (int) (y >> 16);
      *x++ = y & 0xffff;
#endif
    }
  while (++i < wds);
 804dea9:	40                   	inc    %eax
 804deaa:	39 44 24 1c          	cmp    %eax,0x1c(%esp)
 804deae:	7f cc                	jg     804de7c <__multadd+0x24>
  if (a)
 804deb0:	85 ff                	test   %edi,%edi
 804deb2:	74 23                	je     804ded7 <__multadd+0x7f>
    {
      if (wds >= b->_maxwds)
 804deb4:	8b 54 24 1c          	mov    0x1c(%esp),%edx
 804deb8:	8b 44 24 44          	mov    0x44(%esp),%eax
 804debc:	3b 50 08             	cmp    0x8(%eax),%edx
 804debf:	7d 23                	jge    804dee4 <__multadd+0x8c>
	  b1 = Balloc (ptr, b->_k + 1);
	  Bcopy (b1, b);
	  Bfree (ptr, b);
	  b = b1;
	}
      b->_x[wds++] = a;
 804dec1:	8b 54 24 1c          	mov    0x1c(%esp),%edx
 804dec5:	8b 44 24 44          	mov    0x44(%esp),%eax
 804dec9:	89 7c 90 14          	mov    %edi,0x14(%eax,%edx,4)
 804decd:	89 d0                	mov    %edx,%eax
 804decf:	40                   	inc    %eax
 804ded0:	8b 54 24 44          	mov    0x44(%esp),%edx
 804ded4:	89 42 10             	mov    %eax,0x10(%edx)
      b->_wds = wds;
    }
  return b;
}
 804ded7:	8b 44 24 44          	mov    0x44(%esp),%eax
 804dedb:	83 c4 2c             	add    $0x2c,%esp
 804dede:	5b                   	pop    %ebx
 804dedf:	5e                   	pop    %esi
 804dee0:	5f                   	pop    %edi
 804dee1:	5d                   	pop    %ebp
 804dee2:	c3                   	ret    
 804dee3:	90                   	nop
  while (++i < wds);
  if (a)
    {
      if (wds >= b->_maxwds)
	{
	  b1 = Balloc (ptr, b->_k + 1);
 804dee4:	8b 48 04             	mov    0x4(%eax),%ecx
 804dee7:	89 c8                	mov    %ecx,%eax
 804dee9:	40                   	inc    %eax
 804deea:	89 44 24 04          	mov    %eax,0x4(%esp)
 804deee:	8b 44 24 40          	mov    0x40(%esp),%eax
 804def2:	89 04 24             	mov    %eax,(%esp)
 804def5:	e8 b6 fe ff ff       	call   804ddb0 <_Balloc>
 804defa:	89 c3                	mov    %eax,%ebx
	  Bcopy (b1, b);
 804defc:	8b 54 24 44          	mov    0x44(%esp),%edx
 804df00:	8b 42 10             	mov    0x10(%edx),%eax
 804df03:	8d 04 85 08 00 00 00 	lea    0x8(,%eax,4),%eax
 804df0a:	89 44 24 08          	mov    %eax,0x8(%esp)
 804df0e:	89 d0                	mov    %edx,%eax
 804df10:	83 c0 0c             	add    $0xc,%eax
 804df13:	89 44 24 04          	mov    %eax,0x4(%esp)
 804df17:	8d 43 0c             	lea    0xc(%ebx),%eax
 804df1a:	89 04 24             	mov    %eax,(%esp)
 804df1d:	e8 26 fd ff ff       	call   804dc48 <memcpy>
_DEFUN (Bfree, (ptr, v), struct _reent *ptr _AND _Bigint * v)
{
  _REENT_CHECK_MP(ptr);
  if (v)
    {
      v->_next = _REENT_MP_FREELIST(ptr)[v->_k];
 804df22:	8b 44 24 44          	mov    0x44(%esp),%eax
 804df26:	8b 50 04             	mov    0x4(%eax),%edx
 804df29:	8b 4c 24 40          	mov    0x40(%esp),%ecx
 804df2d:	8b 41 4c             	mov    0x4c(%ecx),%eax
 804df30:	8d 04 90             	lea    (%eax,%edx,4),%eax
 804df33:	8b 10                	mov    (%eax),%edx
 804df35:	8b 4c 24 44          	mov    0x44(%esp),%ecx
 804df39:	89 11                	mov    %edx,(%ecx)
      _REENT_MP_FREELIST(ptr)[v->_k] = v;
 804df3b:	89 08                	mov    %ecx,(%eax)
      if (wds >= b->_maxwds)
	{
	  b1 = Balloc (ptr, b->_k + 1);
	  Bcopy (b1, b);
	  Bfree (ptr, b);
	  b = b1;
 804df3d:	89 5c 24 44          	mov    %ebx,0x44(%esp)
 804df41:	e9 7b ff ff ff       	jmp    804dec1 <__multadd+0x69>
 804df46:	66 90                	xchg   %ax,%ax

0804df48 <__s2b>:
	struct _reent * ptr _AND
	_CONST char *s _AND
	int nd0 _AND
	int nd _AND
	__ULong y9)
{
 804df48:	55                   	push   %ebp
 804df49:	57                   	push   %edi
 804df4a:	56                   	push   %esi
 804df4b:	53                   	push   %ebx
 804df4c:	83 ec 1c             	sub    $0x1c,%esp
 804df4f:	8b 5c 24 30          	mov    0x30(%esp),%ebx
 804df53:	8b 74 24 34          	mov    0x34(%esp),%esi
  _Bigint *b;
  int i, k;
  __Long x, y;

  x = (nd + 8) / 9;
 804df57:	8b 4c 24 3c          	mov    0x3c(%esp),%ecx
 804df5b:	83 c1 08             	add    $0x8,%ecx
 804df5e:	ba 39 8e e3 38       	mov    $0x38e38e39,%edx
 804df63:	89 c8                	mov    %ecx,%eax
 804df65:	f7 ea                	imul   %edx
 804df67:	d1 fa                	sar    %edx
 804df69:	c1 f9 1f             	sar    $0x1f,%ecx
 804df6c:	29 ca                	sub    %ecx,%edx
  for (k = 0, y = 1; x > y; y <<= 1, k++);
 804df6e:	83 fa 01             	cmp    $0x1,%edx
 804df71:	0f 8e af 00 00 00    	jle    804e026 <__s2b+0xde>
 804df77:	b8 01 00 00 00       	mov    $0x1,%eax
 804df7c:	31 c9                	xor    %ecx,%ecx
 804df7e:	66 90                	xchg   %ax,%ax
 804df80:	d1 e0                	shl    %eax
 804df82:	41                   	inc    %ecx
 804df83:	39 c2                	cmp    %eax,%edx
 804df85:	7f f9                	jg     804df80 <__s2b+0x38>
#ifdef Pack_32
  b = Balloc (ptr, k);
 804df87:	89 4c 24 04          	mov    %ecx,0x4(%esp)
 804df8b:	89 1c 24             	mov    %ebx,(%esp)
 804df8e:	e8 1d fe ff ff       	call   804ddb0 <_Balloc>
  b->_x[0] = y9;
 804df93:	8b 54 24 40          	mov    0x40(%esp),%edx
 804df97:	89 50 14             	mov    %edx,0x14(%eax)
  b->_wds = 1;
 804df9a:	c7 40 10 01 00 00 00 	movl   $0x1,0x10(%eax)
  b->_x[0] = y9 & 0xffff;
  b->_wds = (b->_x[1] = y9 >> 16) ? 2 : 1;
#endif

  i = 9;
  if (9 < nd0)
 804dfa1:	83 7c 24 38 09       	cmpl   $0x9,0x38(%esp)
 804dfa6:	7e 74                	jle    804e01c <__s2b+0xd4>
    {
      s += 9;
 804dfa8:	8d 6e 09             	lea    0x9(%esi),%ebp
    }
  return b;
}

_Bigint *
_DEFUN (s2b, (ptr, s, nd0, nd, y9),
 804dfab:	03 74 24 38          	add    0x38(%esp),%esi
#endif

  i = 9;
  if (9 < nd0)
    {
      s += 9;
 804dfaf:	89 ef                	mov    %ebp,%edi
 804dfb1:	8d 76 00             	lea    0x0(%esi),%esi
      do
	b = multadd (ptr, b, 10, *s++ - '0');
 804dfb4:	0f be 17             	movsbl (%edi),%edx
 804dfb7:	83 ea 30             	sub    $0x30,%edx
 804dfba:	47                   	inc    %edi
 804dfbb:	89 54 24 0c          	mov    %edx,0xc(%esp)
 804dfbf:	c7 44 24 08 0a 00 00 	movl   $0xa,0x8(%esp)
 804dfc6:	00 
 804dfc7:	89 44 24 04          	mov    %eax,0x4(%esp)
 804dfcb:	89 1c 24             	mov    %ebx,(%esp)
 804dfce:	e8 85 fe ff ff       	call   804de58 <__multadd>
      while (++i < nd0);
 804dfd3:	39 f7                	cmp    %esi,%edi
 804dfd5:	75 dd                	jne    804dfb4 <__s2b+0x6c>
      s++;
 804dfd7:	8b 54 24 38          	mov    0x38(%esp),%edx
 804dfdb:	8d 74 15 f8          	lea    -0x8(%ebp,%edx,1),%esi
    }
  else
    s += 10;
  for (; i < nd; i++)
 804dfdf:	39 54 24 3c          	cmp    %edx,0x3c(%esp)
 804dfe3:	7e 2e                	jle    804e013 <__s2b+0xcb>
    }
  return b;
}

_Bigint *
_DEFUN (s2b, (ptr, s, nd0, nd, y9),
 804dfe5:	8b 7c 24 3c          	mov    0x3c(%esp),%edi
 804dfe9:	29 d7                	sub    %edx,%edi
 804dfeb:	01 f7                	add    %esi,%edi
 804dfed:	8d 76 00             	lea    0x0(%esi),%esi
      s++;
    }
  else
    s += 10;
  for (; i < nd; i++)
    b = multadd (ptr, b, 10, *s++ - '0');
 804dff0:	0f be 16             	movsbl (%esi),%edx
 804dff3:	83 ea 30             	sub    $0x30,%edx
 804dff6:	46                   	inc    %esi
 804dff7:	89 54 24 0c          	mov    %edx,0xc(%esp)
 804dffb:	c7 44 24 08 0a 00 00 	movl   $0xa,0x8(%esp)
 804e002:	00 
 804e003:	89 44 24 04          	mov    %eax,0x4(%esp)
 804e007:	89 1c 24             	mov    %ebx,(%esp)
 804e00a:	e8 49 fe ff ff       	call   804de58 <__multadd>
      while (++i < nd0);
      s++;
    }
  else
    s += 10;
  for (; i < nd; i++)
 804e00f:	39 fe                	cmp    %edi,%esi
 804e011:	75 dd                	jne    804dff0 <__s2b+0xa8>
    b = multadd (ptr, b, 10, *s++ - '0');
  return b;
}
 804e013:	83 c4 1c             	add    $0x1c,%esp
 804e016:	5b                   	pop    %ebx
 804e017:	5e                   	pop    %esi
 804e018:	5f                   	pop    %edi
 804e019:	5d                   	pop    %ebp
 804e01a:	c3                   	ret    
 804e01b:	90                   	nop
	b = multadd (ptr, b, 10, *s++ - '0');
      while (++i < nd0);
      s++;
    }
  else
    s += 10;
 804e01c:	83 c6 0a             	add    $0xa,%esi
  b = Balloc (ptr, k + 1);
  b->_x[0] = y9 & 0xffff;
  b->_wds = (b->_x[1] = y9 >> 16) ? 2 : 1;
#endif

  i = 9;
 804e01f:	ba 09 00 00 00       	mov    $0x9,%edx
 804e024:	eb b9                	jmp    804dfdf <__s2b+0x97>
  _Bigint *b;
  int i, k;
  __Long x, y;

  x = (nd + 8) / 9;
  for (k = 0, y = 1; x > y; y <<= 1, k++);
 804e026:	31 c9                	xor    %ecx,%ecx
 804e028:	e9 5a ff ff ff       	jmp    804df87 <__s2b+0x3f>
 804e02d:	8d 76 00             	lea    0x0(%esi),%esi

0804e030 <__hi0bits>:
}

int
_DEFUN (hi0bits,
	(x), register __ULong x)
{
 804e030:	8b 54 24 04          	mov    0x4(%esp),%edx
  register int k = 0;

  if (!(x & 0xffff0000))
 804e034:	f7 c2 00 00 ff ff    	test   $0xffff0000,%edx
 804e03a:	75 44                	jne    804e080 <__hi0bits+0x50>
    {
      k = 16;
      x <<= 16;
 804e03c:	c1 e2 10             	shl    $0x10,%edx
{
  register int k = 0;

  if (!(x & 0xffff0000))
    {
      k = 16;
 804e03f:	b8 10 00 00 00       	mov    $0x10,%eax
      x <<= 16;
    }
  if (!(x & 0xff000000))
 804e044:	f7 c2 00 00 00 ff    	test   $0xff000000,%edx
 804e04a:	75 06                	jne    804e052 <__hi0bits+0x22>
    {
      k += 8;
 804e04c:	83 c0 08             	add    $0x8,%eax
      x <<= 8;
 804e04f:	c1 e2 08             	shl    $0x8,%edx
    }
  if (!(x & 0xf0000000))
 804e052:	f7 c2 00 00 00 f0    	test   $0xf0000000,%edx
 804e058:	75 06                	jne    804e060 <__hi0bits+0x30>
    {
      k += 4;
 804e05a:	83 c0 04             	add    $0x4,%eax
      x <<= 4;
 804e05d:	c1 e2 04             	shl    $0x4,%edx
    }
  if (!(x & 0xc0000000))
 804e060:	f7 c2 00 00 00 c0    	test   $0xc0000000,%edx
 804e066:	75 06                	jne    804e06e <__hi0bits+0x3e>
    {
      k += 2;
 804e068:	83 c0 02             	add    $0x2,%eax
      x <<= 2;
 804e06b:	c1 e2 02             	shl    $0x2,%edx
    }
  if (!(x & 0x80000000))
 804e06e:	85 d2                	test   %edx,%edx
 804e070:	78 0d                	js     804e07f <__hi0bits+0x4f>
    {
      k++;
      if (!(x & 0x40000000))
 804e072:	81 e2 00 00 00 40    	and    $0x40000000,%edx
 804e078:	75 0a                	jne    804e084 <__hi0bits+0x54>
	return 32;
 804e07a:	b8 20 00 00 00       	mov    $0x20,%eax
    }
  return k;
}
 804e07f:	c3                   	ret    

int
_DEFUN (hi0bits,
	(x), register __ULong x)
{
  register int k = 0;
 804e080:	31 c0                	xor    %eax,%eax
 804e082:	eb c0                	jmp    804e044 <__hi0bits+0x14>
      k += 2;
      x <<= 2;
    }
  if (!(x & 0x80000000))
    {
      k++;
 804e084:	40                   	inc    %eax
 804e085:	c3                   	ret    
 804e086:	66 90                	xchg   %ax,%ax

0804e088 <__lo0bits>:
  return k;
}

int
_DEFUN (lo0bits, (y), __ULong *y)
{
 804e088:	8b 4c 24 04          	mov    0x4(%esp),%ecx
  register int k;
  register __ULong x = *y;
 804e08c:	8b 11                	mov    (%ecx),%edx

  if (x & 7)
 804e08e:	f6 c2 07             	test   $0x7,%dl
 804e091:	74 15                	je     804e0a8 <__lo0bits+0x20>
    {
      if (x & 1)
 804e093:	f6 c2 01             	test   $0x1,%dl
 804e096:	75 58                	jne    804e0f0 <__lo0bits+0x68>
	return 0;
      if (x & 2)
 804e098:	f6 c2 02             	test   $0x2,%dl
 804e09b:	75 57                	jne    804e0f4 <__lo0bits+0x6c>
	{
	  *y = x >> 1;
	  return 1;
	}
      *y = x >> 2;
 804e09d:	c1 ea 02             	shr    $0x2,%edx
 804e0a0:	89 11                	mov    %edx,(%ecx)
      return 2;
 804e0a2:	b8 02 00 00 00       	mov    $0x2,%eax
 804e0a7:	c3                   	ret    
    }
  k = 0;
  if (!(x & 0xffff))
 804e0a8:	66 85 d2             	test   %dx,%dx
 804e0ab:	74 33                	je     804e0e0 <__lo0bits+0x58>
	  return 1;
	}
      *y = x >> 2;
      return 2;
    }
  k = 0;
 804e0ad:	31 c0                	xor    %eax,%eax
  if (!(x & 0xffff))
    {
      k = 16;
      x >>= 16;
    }
  if (!(x & 0xff))
 804e0af:	84 d2                	test   %dl,%dl
 804e0b1:	75 06                	jne    804e0b9 <__lo0bits+0x31>
    {
      k += 8;
 804e0b3:	83 c0 08             	add    $0x8,%eax
      x >>= 8;
 804e0b6:	c1 ea 08             	shr    $0x8,%edx
    }
  if (!(x & 0xf))
 804e0b9:	f6 c2 0f             	test   $0xf,%dl
 804e0bc:	75 06                	jne    804e0c4 <__lo0bits+0x3c>
    {
      k += 4;
 804e0be:	83 c0 04             	add    $0x4,%eax
      x >>= 4;
 804e0c1:	c1 ea 04             	shr    $0x4,%edx
    }
  if (!(x & 0x3))
 804e0c4:	f6 c2 03             	test   $0x3,%dl
 804e0c7:	75 06                	jne    804e0cf <__lo0bits+0x47>
    {
      k += 2;
 804e0c9:	83 c0 02             	add    $0x2,%eax
      x >>= 2;
 804e0cc:	c1 ea 02             	shr    $0x2,%edx
    }
  if (!(x & 1))
 804e0cf:	f6 c2 01             	test   $0x1,%dl
 804e0d2:	75 19                	jne    804e0ed <__lo0bits+0x65>
    {
      k++;
      x >>= 1;
      if (!x & 1)
 804e0d4:	d1 ea                	shr    %edx
 804e0d6:	75 14                	jne    804e0ec <__lo0bits+0x64>
	return 32;
 804e0d8:	b8 20 00 00 00       	mov    $0x20,%eax
    }
  *y = x;
  return k;
}
 804e0dd:	c3                   	ret    
 804e0de:	66 90                	xchg   %ax,%ax
    }
  k = 0;
  if (!(x & 0xffff))
    {
      k = 16;
      x >>= 16;
 804e0e0:	c1 ea 10             	shr    $0x10,%edx
      return 2;
    }
  k = 0;
  if (!(x & 0xffff))
    {
      k = 16;
 804e0e3:	b8 10 00 00 00       	mov    $0x10,%eax
 804e0e8:	eb c5                	jmp    804e0af <__lo0bits+0x27>
 804e0ea:	66 90                	xchg   %ax,%ax
      k += 2;
      x >>= 2;
    }
  if (!(x & 1))
    {
      k++;
 804e0ec:	40                   	inc    %eax
      x >>= 1;
      if (!x & 1)
	return 32;
    }
  *y = x;
 804e0ed:	89 11                	mov    %edx,(%ecx)
  return k;
 804e0ef:	c3                   	ret    
  register __ULong x = *y;

  if (x & 7)
    {
      if (x & 1)
	return 0;
 804e0f0:	31 c0                	xor    %eax,%eax
 804e0f2:	c3                   	ret    
 804e0f3:	90                   	nop
      if (x & 2)
	{
	  *y = x >> 1;
 804e0f4:	d1 ea                	shr    %edx
 804e0f6:	89 11                	mov    %edx,(%ecx)
	  return 1;
 804e0f8:	b8 01 00 00 00       	mov    $0x1,%eax
 804e0fd:	c3                   	ret    
 804e0fe:	66 90                	xchg   %ax,%ax

0804e100 <__i2b>:
  return k;
}

_Bigint *
_DEFUN (i2b, (ptr, i), struct _reent * ptr _AND int i)
{
 804e100:	83 ec 1c             	sub    $0x1c,%esp
  _Bigint *b;

  b = Balloc (ptr, 1);
 804e103:	c7 44 24 04 01 00 00 	movl   $0x1,0x4(%esp)
 804e10a:	00 
 804e10b:	8b 44 24 20          	mov    0x20(%esp),%eax
 804e10f:	89 04 24             	mov    %eax,(%esp)
 804e112:	e8 99 fc ff ff       	call   804ddb0 <_Balloc>
  b->_x[0] = i;
 804e117:	8b 54 24 24          	mov    0x24(%esp),%edx
 804e11b:	89 50 14             	mov    %edx,0x14(%eax)
  b->_wds = 1;
 804e11e:	c7 40 10 01 00 00 00 	movl   $0x1,0x10(%eax)
  return b;
}
 804e125:	83 c4 1c             	add    $0x1c,%esp
 804e128:	c3                   	ret    
 804e129:	8d 76 00             	lea    0x0(%esi),%esi

0804e12c <__multiply>:

_Bigint *
_DEFUN (mult, (ptr, a, b), struct _reent * ptr _AND _Bigint * a _AND _Bigint * b)
{
 804e12c:	55                   	push   %ebp
 804e12d:	57                   	push   %edi
 804e12e:	56                   	push   %esi
 804e12f:	53                   	push   %ebx
 804e130:	83 ec 3c             	sub    $0x3c,%esp
 804e133:	8b 5c 24 54          	mov    0x54(%esp),%ebx
 804e137:	8b 7c 24 58          	mov    0x58(%esp),%edi
  __ULong *x, *xa, *xae, *xb, *xbe, *xc, *xc0;
#ifdef Pack_32
  __ULong z2;
#endif

  if (a->_wds < b->_wds)
 804e13b:	8b 6b 10             	mov    0x10(%ebx),%ebp
 804e13e:	8b 77 10             	mov    0x10(%edi),%esi
 804e141:	39 f5                	cmp    %esi,%ebp
 804e143:	7d 0c                	jge    804e151 <__multiply+0x25>
 804e145:	89 e8                	mov    %ebp,%eax
 804e147:	89 f5                	mov    %esi,%ebp
 804e149:	89 c6                	mov    %eax,%esi
 804e14b:	89 d8                	mov    %ebx,%eax
 804e14d:	89 fb                	mov    %edi,%ebx
 804e14f:	89 c7                	mov    %eax,%edi
    {
      c = a;
      a = b;
      b = c;
    }
  k = a->_k;
 804e151:	8b 43 04             	mov    0x4(%ebx),%eax
  wa = a->_wds;
  wb = b->_wds;
  wc = wa + wb;
 804e154:	8d 54 35 00          	lea    0x0(%ebp,%esi,1),%edx
 804e158:	89 54 24 24          	mov    %edx,0x24(%esp)
  if (wc > a->_maxwds)
 804e15c:	3b 53 08             	cmp    0x8(%ebx),%edx
 804e15f:	7e 01                	jle    804e162 <__multiply+0x36>
    k++;
 804e161:	40                   	inc    %eax
  c = Balloc (ptr, k);
 804e162:	89 44 24 04          	mov    %eax,0x4(%esp)
 804e166:	8b 44 24 50          	mov    0x50(%esp),%eax
 804e16a:	89 04 24             	mov    %eax,(%esp)
 804e16d:	e8 3e fc ff ff       	call   804ddb0 <_Balloc>
 804e172:	89 44 24 28          	mov    %eax,0x28(%esp)
  for (x = c->_x, xa = x + wc; x < xa; x++)
 804e176:	83 c0 14             	add    $0x14,%eax
 804e179:	8b 4c 24 24          	mov    0x24(%esp),%ecx
 804e17d:	8b 54 24 28          	mov    0x28(%esp),%edx
 804e181:	8d 4c 8a 14          	lea    0x14(%edx,%ecx,4),%ecx
 804e185:	89 4c 24 2c          	mov    %ecx,0x2c(%esp)
 804e189:	89 c2                	mov    %eax,%edx
 804e18b:	39 c8                	cmp    %ecx,%eax
 804e18d:	73 12                	jae    804e1a1 <__multiply+0x75>
 804e18f:	8b 4c 24 2c          	mov    0x2c(%esp),%ecx
 804e193:	90                   	nop
    *x = 0;
 804e194:	c7 02 00 00 00 00    	movl   $0x0,(%edx)
  wb = b->_wds;
  wc = wa + wb;
  if (wc > a->_maxwds)
    k++;
  c = Balloc (ptr, k);
  for (x = c->_x, xa = x + wc; x < xa; x++)
 804e19a:	83 c2 04             	add    $0x4,%edx
 804e19d:	39 d1                	cmp    %edx,%ecx
 804e19f:	77 f3                	ja     804e194 <__multiply+0x68>
    *x = 0;
  xa = a->_x;
 804e1a1:	8d 53 14             	lea    0x14(%ebx),%edx
 804e1a4:	89 54 24 20          	mov    %edx,0x20(%esp)
  xae = xa + wa;
 804e1a8:	8d 5c ab 14          	lea    0x14(%ebx,%ebp,4),%ebx
 804e1ac:	89 5c 24 10          	mov    %ebx,0x10(%esp)
  xb = b->_x;
 804e1b0:	8d 4f 14             	lea    0x14(%edi),%ecx
 804e1b3:	89 4c 24 14          	mov    %ecx,0x14(%esp)
  xbe = xb + wb;
 804e1b7:	8d 7c b7 14          	lea    0x14(%edi,%esi,4),%edi
 804e1bb:	89 7c 24 1c          	mov    %edi,0x1c(%esp)
  xc0 = c->_x;
#ifdef Pack_32
  for (; xb < xbe; xb++, xc0++)
 804e1bf:	89 44 24 18          	mov    %eax,0x18(%esp)
 804e1c3:	39 f9                	cmp    %edi,%ecx
 804e1c5:	0f 83 db 00 00 00    	jae    804e2a6 <__multiply+0x17a>
 804e1cb:	90                   	nop
    {
      if ((y = *xb & 0xffff) != 0)
 804e1cc:	8b 5c 24 14          	mov    0x14(%esp),%ebx
 804e1d0:	8b 3b                	mov    (%ebx),%edi
 804e1d2:	89 fe                	mov    %edi,%esi
 804e1d4:	81 e6 ff ff 00 00    	and    $0xffff,%esi
 804e1da:	74 55                	je     804e231 <__multiply+0x105>
 804e1dc:	8b 44 24 18          	mov    0x18(%esp),%eax
 804e1e0:	8b 54 24 20          	mov    0x20(%esp),%edx
 804e1e4:	31 ff                	xor    %edi,%edi
 804e1e6:	66 90                	xchg   %ax,%ax
	  x = xa;
	  xc = xc0;
	  carry = 0;
	  do
	    {
	      z = (*x & 0xffff) * y + (*xc & 0xffff) + carry;
 804e1e8:	8b 1a                	mov    (%edx),%ebx
 804e1ea:	0f b7 cb             	movzwl %bx,%ecx
 804e1ed:	0f af ce             	imul   %esi,%ecx
 804e1f0:	0f b7 28             	movzwl (%eax),%ebp
 804e1f3:	01 e9                	add    %ebp,%ecx
 804e1f5:	01 f9                	add    %edi,%ecx
	      carry = z >> 16;
	      z2 = (*x++ >> 16) * y + (*xc >> 16) + carry;
 804e1f7:	c1 eb 10             	shr    $0x10,%ebx
 804e1fa:	0f af de             	imul   %esi,%ebx
 804e1fd:	8b 38                	mov    (%eax),%edi
 804e1ff:	c1 ef 10             	shr    $0x10,%edi
 804e202:	01 fb                	add    %edi,%ebx
	  xc = xc0;
	  carry = 0;
	  do
	    {
	      z = (*x & 0xffff) * y + (*xc & 0xffff) + carry;
	      carry = z >> 16;
 804e204:	89 cf                	mov    %ecx,%edi
 804e206:	c1 ef 10             	shr    $0x10,%edi
	      z2 = (*x++ >> 16) * y + (*xc >> 16) + carry;
 804e209:	01 fb                	add    %edi,%ebx
 804e20b:	83 c2 04             	add    $0x4,%edx
	      carry = z2 >> 16;
 804e20e:	89 df                	mov    %ebx,%edi
 804e210:	c1 ef 10             	shr    $0x10,%edi
	      Storeinc (xc, z2, z);
 804e213:	c1 e3 10             	shl    $0x10,%ebx
 804e216:	81 e1 ff ff 00 00    	and    $0xffff,%ecx
 804e21c:	09 cb                	or     %ecx,%ebx
 804e21e:	89 18                	mov    %ebx,(%eax)
 804e220:	83 c0 04             	add    $0x4,%eax
	    }
	  while (x < xae);
 804e223:	39 54 24 10          	cmp    %edx,0x10(%esp)
 804e227:	77 bf                	ja     804e1e8 <__multiply+0xbc>
	  *xc = carry;
 804e229:	89 38                	mov    %edi,(%eax)
 804e22b:	8b 44 24 14          	mov    0x14(%esp),%eax
 804e22f:	8b 38                	mov    (%eax),%edi
	}
      if ((y = *xb >> 16) != 0)
 804e231:	c1 ef 10             	shr    $0x10,%edi
 804e234:	74 58                	je     804e28e <__multiply+0x162>
	{
	  x = xa;
	  xc = xc0;
	  carry = 0;
	  z2 = *xc;
 804e236:	8b 54 24 18          	mov    0x18(%esp),%edx
 804e23a:	8b 02                	mov    (%edx),%eax
 804e23c:	89 c6                	mov    %eax,%esi
 804e23e:	89 d1                	mov    %edx,%ecx
	  while (x < xae);
	  *xc = carry;
	}
      if ((y = *xb >> 16) != 0)
	{
	  x = xa;
 804e240:	8b 54 24 20          	mov    0x20(%esp),%edx
	  xc = xc0;
	  carry = 0;
 804e244:	31 ed                	xor    %ebp,%ebp
 804e246:	66 90                	xchg   %ax,%ax
	  z2 = *xc;
	  do
	    {
	      z = (*x & 0xffff) * y + (*xc >> 16) + carry;
 804e248:	0f b7 1a             	movzwl (%edx),%ebx
 804e24b:	0f af df             	imul   %edi,%ebx
 804e24e:	c1 ee 10             	shr    $0x10,%esi
 804e251:	01 de                	add    %ebx,%esi
 804e253:	01 f5                	add    %esi,%ebp
	      carry = z >> 16;
	      Storeinc (xc, z, z2);
 804e255:	89 ee                	mov    %ebp,%esi
 804e257:	c1 e6 10             	shl    $0x10,%esi
 804e25a:	25 ff ff 00 00       	and    $0xffff,%eax
 804e25f:	09 c6                	or     %eax,%esi
 804e261:	89 31                	mov    %esi,(%ecx)
 804e263:	83 c1 04             	add    $0x4,%ecx
	      z2 = (*x++ >> 16) * y + (*xc & 0xffff) + carry;
 804e266:	8b 31                	mov    (%ecx),%esi
 804e268:	89 f0                	mov    %esi,%eax
 804e26a:	25 ff ff 00 00       	and    $0xffff,%eax
 804e26f:	8b 1a                	mov    (%edx),%ebx
 804e271:	c1 eb 10             	shr    $0x10,%ebx
 804e274:	0f af df             	imul   %edi,%ebx
 804e277:	01 d8                	add    %ebx,%eax
	  carry = 0;
	  z2 = *xc;
	  do
	    {
	      z = (*x & 0xffff) * y + (*xc >> 16) + carry;
	      carry = z >> 16;
 804e279:	c1 ed 10             	shr    $0x10,%ebp
	      Storeinc (xc, z, z2);
	      z2 = (*x++ >> 16) * y + (*xc & 0xffff) + carry;
 804e27c:	01 e8                	add    %ebp,%eax
 804e27e:	83 c2 04             	add    $0x4,%edx
	      carry = z2 >> 16;
 804e281:	89 c5                	mov    %eax,%ebp
 804e283:	c1 ed 10             	shr    $0x10,%ebp
	    }
	  while (x < xae);
 804e286:	39 54 24 10          	cmp    %edx,0x10(%esp)
 804e28a:	77 bc                	ja     804e248 <__multiply+0x11c>
	  *xc = z2;
 804e28c:	89 01                	mov    %eax,(%ecx)
  xae = xa + wa;
  xb = b->_x;
  xbe = xb + wb;
  xc0 = c->_x;
#ifdef Pack_32
  for (; xb < xbe; xb++, xc0++)
 804e28e:	83 44 24 14 04       	addl   $0x4,0x14(%esp)
 804e293:	83 44 24 18 04       	addl   $0x4,0x18(%esp)
 804e298:	8b 4c 24 14          	mov    0x14(%esp),%ecx
 804e29c:	39 4c 24 1c          	cmp    %ecx,0x1c(%esp)
 804e2a0:	0f 87 26 ff ff ff    	ja     804e1cc <__multiply+0xa0>
	  while (x < xae);
	  *xc = carry;
	}
    }
#endif
  for (xc0 = c->_x, xc = xc0 + wc; wc > 0 && !*--xc; --wc);
 804e2a6:	8b 7c 24 24          	mov    0x24(%esp),%edi
 804e2aa:	85 ff                	test   %edi,%edi
 804e2ac:	7e 29                	jle    804e2d7 <__multiply+0x1ab>
 804e2ae:	8b 5c 24 2c          	mov    0x2c(%esp),%ebx
 804e2b2:	8b 73 fc             	mov    -0x4(%ebx),%esi
 804e2b5:	85 f6                	test   %esi,%esi
 804e2b7:	75 1e                	jne    804e2d7 <__multiply+0x1ab>
  b->_wds = 1;
  return b;
}

_Bigint *
_DEFUN (mult, (ptr, a, b), struct _reent * ptr _AND _Bigint * a _AND _Bigint * b)
 804e2b9:	8b 44 24 24          	mov    0x24(%esp),%eax
 804e2bd:	f7 d8                	neg    %eax
 804e2bf:	8d 04 83             	lea    (%ebx,%eax,4),%eax
 804e2c2:	8b 54 24 24          	mov    0x24(%esp),%edx
 804e2c6:	eb 08                	jmp    804e2d0 <__multiply+0x1a4>
	  while (x < xae);
	  *xc = carry;
	}
    }
#endif
  for (xc0 = c->_x, xc = xc0 + wc; wc > 0 && !*--xc; --wc);
 804e2c8:	8b 4c 90 fc          	mov    -0x4(%eax,%edx,4),%ecx
 804e2cc:	85 c9                	test   %ecx,%ecx
 804e2ce:	75 03                	jne    804e2d3 <__multiply+0x1a7>
 804e2d0:	4a                   	dec    %edx
 804e2d1:	75 f5                	jne    804e2c8 <__multiply+0x19c>
 804e2d3:	89 54 24 24          	mov    %edx,0x24(%esp)
  c->_wds = wc;
 804e2d7:	8b 54 24 24          	mov    0x24(%esp),%edx
 804e2db:	8b 44 24 28          	mov    0x28(%esp),%eax
 804e2df:	89 50 10             	mov    %edx,0x10(%eax)
  return c;
}
 804e2e2:	83 c4 3c             	add    $0x3c,%esp
 804e2e5:	5b                   	pop    %ebx
 804e2e6:	5e                   	pop    %esi
 804e2e7:	5f                   	pop    %edi
 804e2e8:	5d                   	pop    %ebp
 804e2e9:	c3                   	ret    
 804e2ea:	66 90                	xchg   %ax,%ax

0804e2ec <__pow5mult>:

_Bigint *
_DEFUN (pow5mult,
	(ptr, b, k), struct _reent * ptr _AND _Bigint * b _AND int k)
{
 804e2ec:	55                   	push   %ebp
 804e2ed:	57                   	push   %edi
 804e2ee:	56                   	push   %esi
 804e2ef:	53                   	push   %ebx
 804e2f0:	83 ec 1c             	sub    $0x1c,%esp
 804e2f3:	8b 6c 24 30          	mov    0x30(%esp),%ebp
 804e2f7:	8b 7c 24 34          	mov    0x34(%esp),%edi
 804e2fb:	8b 5c 24 38          	mov    0x38(%esp),%ebx
  _Bigint *b1, *p5, *p51;
  int i;
  static _CONST int p05[3] = {5, 25, 125};

  if ((i = k & 3) != 0)
 804e2ff:	89 d8                	mov    %ebx,%eax
 804e301:	83 e0 03             	and    $0x3,%eax
 804e304:	75 7a                	jne    804e380 <__pow5mult+0x94>
    b = multadd (ptr, b, p05[i - 1], 0);

  if (!(k >>= 2))
 804e306:	c1 fb 02             	sar    $0x2,%ebx
 804e309:	74 49                	je     804e354 <__pow5mult+0x68>
    return b;
  _REENT_CHECK_MP(ptr);
  if (!(p5 = _REENT_MP_P5S(ptr)))
 804e30b:	8b 75 48             	mov    0x48(%ebp),%esi
 804e30e:	85 f6                	test   %esi,%esi
 804e310:	75 12                	jne    804e324 <__pow5mult+0x38>
 804e312:	e9 8f 00 00 00       	jmp    804e3a6 <__pow5mult+0xba>
 804e317:	90                   	nop
	{
	  b1 = mult (ptr, b, p5);
	  Bfree (ptr, b);
	  b = b1;
	}
      if (!(k >>= 1))
 804e318:	d1 fb                	sar    %ebx
 804e31a:	74 38                	je     804e354 <__pow5mult+0x68>
	break;
      if (!(p51 = p5->_next))
 804e31c:	8b 16                	mov    (%esi),%edx
 804e31e:	85 d2                	test   %edx,%edx
 804e320:	74 3e                	je     804e360 <__pow5mult+0x74>
 804e322:	89 d6                	mov    %edx,%esi
      p5 = _REENT_MP_P5S(ptr) = i2b (ptr, 625);
      p5->_next = 0;
    }
  for (;;)
    {
      if (k & 1)
 804e324:	f6 c3 01             	test   $0x1,%bl
 804e327:	74 ef                	je     804e318 <__pow5mult+0x2c>
	{
	  b1 = mult (ptr, b, p5);
 804e329:	89 74 24 08          	mov    %esi,0x8(%esp)
 804e32d:	89 7c 24 04          	mov    %edi,0x4(%esp)
 804e331:	89 2c 24             	mov    %ebp,(%esp)
 804e334:	e8 f3 fd ff ff       	call   804e12c <__multiply>

void
_DEFUN (Bfree, (ptr, v), struct _reent *ptr _AND _Bigint * v)
{
  _REENT_CHECK_MP(ptr);
  if (v)
 804e339:	85 ff                	test   %edi,%edi
 804e33b:	74 3f                	je     804e37c <__pow5mult+0x90>
    {
      v->_next = _REENT_MP_FREELIST(ptr)[v->_k];
 804e33d:	8b 4f 04             	mov    0x4(%edi),%ecx
 804e340:	8b 55 4c             	mov    0x4c(%ebp),%edx
 804e343:	8d 14 8a             	lea    (%edx,%ecx,4),%edx
 804e346:	8b 0a                	mov    (%edx),%ecx
 804e348:	89 0f                	mov    %ecx,(%edi)
      _REENT_MP_FREELIST(ptr)[v->_k] = v;
 804e34a:	89 3a                	mov    %edi,(%edx)
    {
      if (k & 1)
	{
	  b1 = mult (ptr, b, p5);
	  Bfree (ptr, b);
	  b = b1;
 804e34c:	89 c7                	mov    %eax,%edi
	}
      if (!(k >>= 1))
 804e34e:	d1 fb                	sar    %ebx
 804e350:	75 ca                	jne    804e31c <__pow5mult+0x30>
 804e352:	66 90                	xchg   %ax,%ax
	  p51->_next = 0;
	}
      p5 = p51;
    }
  return b;
}
 804e354:	89 f8                	mov    %edi,%eax
 804e356:	83 c4 1c             	add    $0x1c,%esp
 804e359:	5b                   	pop    %ebx
 804e35a:	5e                   	pop    %esi
 804e35b:	5f                   	pop    %edi
 804e35c:	5d                   	pop    %ebp
 804e35d:	c3                   	ret    
 804e35e:	66 90                	xchg   %ax,%ax
	}
      if (!(k >>= 1))
	break;
      if (!(p51 = p5->_next))
	{
	  p51 = p5->_next = mult (ptr, p5, p5);
 804e360:	89 74 24 08          	mov    %esi,0x8(%esp)
 804e364:	89 74 24 04          	mov    %esi,0x4(%esp)
 804e368:	89 2c 24             	mov    %ebp,(%esp)
 804e36b:	e8 bc fd ff ff       	call   804e12c <__multiply>
 804e370:	89 06                	mov    %eax,(%esi)
	  p51->_next = 0;
 804e372:	c7 00 00 00 00 00    	movl   $0x0,(%eax)
 804e378:	89 c6                	mov    %eax,%esi
 804e37a:	eb a8                	jmp    804e324 <__pow5mult+0x38>
    {
      if (k & 1)
	{
	  b1 = mult (ptr, b, p5);
	  Bfree (ptr, b);
	  b = b1;
 804e37c:	89 c7                	mov    %eax,%edi
 804e37e:	eb 98                	jmp    804e318 <__pow5mult+0x2c>
  _Bigint *b1, *p5, *p51;
  int i;
  static _CONST int p05[3] = {5, 25, 125};

  if ((i = k & 3) != 0)
    b = multadd (ptr, b, p05[i - 1], 0);
 804e380:	c7 44 24 0c 00 00 00 	movl   $0x0,0xc(%esp)
 804e387:	00 
 804e388:	8b 04 85 e4 19 05 08 	mov    0x80519e4(,%eax,4),%eax
 804e38f:	89 44 24 08          	mov    %eax,0x8(%esp)
 804e393:	89 7c 24 04          	mov    %edi,0x4(%esp)
 804e397:	89 2c 24             	mov    %ebp,(%esp)
 804e39a:	e8 b9 fa ff ff       	call   804de58 <__multadd>
 804e39f:	89 c7                	mov    %eax,%edi
 804e3a1:	e9 60 ff ff ff       	jmp    804e306 <__pow5mult+0x1a>
    return b;
  _REENT_CHECK_MP(ptr);
  if (!(p5 = _REENT_MP_P5S(ptr)))
    {
      /* first time */
      p5 = _REENT_MP_P5S(ptr) = i2b (ptr, 625);
 804e3a6:	c7 44 24 04 71 02 00 	movl   $0x271,0x4(%esp)
 804e3ad:	00 
 804e3ae:	89 2c 24             	mov    %ebp,(%esp)
 804e3b1:	e8 4a fd ff ff       	call   804e100 <__i2b>
 804e3b6:	89 c6                	mov    %eax,%esi
 804e3b8:	89 45 48             	mov    %eax,0x48(%ebp)
      p5->_next = 0;
 804e3bb:	c7 00 00 00 00 00    	movl   $0x0,(%eax)
 804e3c1:	e9 5e ff ff ff       	jmp    804e324 <__pow5mult+0x38>
 804e3c6:	66 90                	xchg   %ax,%ax

0804e3c8 <__lshift>:
  return b;
}

_Bigint *
_DEFUN (lshift, (ptr, b, k), struct _reent * ptr _AND _Bigint * b _AND int k)
{
 804e3c8:	55                   	push   %ebp
 804e3c9:	57                   	push   %edi
 804e3ca:	56                   	push   %esi
 804e3cb:	53                   	push   %ebx
 804e3cc:	83 ec 3c             	sub    $0x3c,%esp
 804e3cf:	8b 7c 24 54          	mov    0x54(%esp),%edi
 804e3d3:	8b 74 24 58          	mov    0x58(%esp),%esi
  int i, k1, n, n1;
  _Bigint *b1;
  __ULong *x, *x1, *xe, z;

#ifdef Pack_32
  n = k >> 5;
 804e3d7:	89 f3                	mov    %esi,%ebx
 804e3d9:	c1 fb 05             	sar    $0x5,%ebx
#else
  n = k >> 4;
#endif
  k1 = b->_k;
 804e3dc:	8b 57 04             	mov    0x4(%edi),%edx
  n1 = n + b->_wds + 1;
 804e3df:	8b 47 10             	mov    0x10(%edi),%eax
 804e3e2:	01 d8                	add    %ebx,%eax
 804e3e4:	89 44 24 2c          	mov    %eax,0x2c(%esp)
 804e3e8:	89 c5                	mov    %eax,%ebp
 804e3ea:	45                   	inc    %ebp
  for (i = b->_maxwds; n1 > i; i <<= 1)
 804e3eb:	8b 47 08             	mov    0x8(%edi),%eax
 804e3ee:	39 c5                	cmp    %eax,%ebp
 804e3f0:	7e 09                	jle    804e3fb <__lshift+0x33>
 804e3f2:	66 90                	xchg   %ax,%ax
    k1++;
 804e3f4:	42                   	inc    %edx
#else
  n = k >> 4;
#endif
  k1 = b->_k;
  n1 = n + b->_wds + 1;
  for (i = b->_maxwds; n1 > i; i <<= 1)
 804e3f5:	d1 e0                	shl    %eax
 804e3f7:	39 c5                	cmp    %eax,%ebp
 804e3f9:	7f f9                	jg     804e3f4 <__lshift+0x2c>
    k1++;
  b1 = Balloc (ptr, k1);
 804e3fb:	89 54 24 04          	mov    %edx,0x4(%esp)
 804e3ff:	8b 54 24 50          	mov    0x50(%esp),%edx
 804e403:	89 14 24             	mov    %edx,(%esp)
 804e406:	e8 a5 f9 ff ff       	call   804ddb0 <_Balloc>
 804e40b:	89 44 24 28          	mov    %eax,0x28(%esp)
  x1 = b1->_x;
 804e40f:	83 c0 14             	add    $0x14,%eax
  for (i = 0; i < n; i++)
 804e412:	85 db                	test   %ebx,%ebx
 804e414:	7e 16                	jle    804e42c <__lshift+0x64>
 804e416:	31 d2                	xor    %edx,%edx
    *x1++ = 0;
 804e418:	c7 04 90 00 00 00 00 	movl   $0x0,(%eax,%edx,4)
  n1 = n + b->_wds + 1;
  for (i = b->_maxwds; n1 > i; i <<= 1)
    k1++;
  b1 = Balloc (ptr, k1);
  x1 = b1->_x;
  for (i = 0; i < n; i++)
 804e41f:	42                   	inc    %edx
 804e420:	39 da                	cmp    %ebx,%edx
 804e422:	75 f4                	jne    804e418 <__lshift+0x50>
 804e424:	8b 4c 24 28          	mov    0x28(%esp),%ecx
 804e428:	8d 44 91 14          	lea    0x14(%ecx,%edx,4),%eax
    *x1++ = 0;
  x = b->_x;
 804e42c:	8d 57 14             	lea    0x14(%edi),%edx
  xe = x + b->_wds;
 804e42f:	8b 4f 10             	mov    0x10(%edi),%ecx
 804e432:	8d 4c 8f 14          	lea    0x14(%edi,%ecx,4),%ecx
 804e436:	89 4c 24 1c          	mov    %ecx,0x1c(%esp)
#ifdef Pack_32
  if (k &= 0x1f)
 804e43a:	83 e6 1f             	and    $0x1f,%esi
 804e43d:	89 74 24 20          	mov    %esi,0x20(%esp)
 804e441:	74 69                	je     804e4ac <__lshift+0xe4>
    {
      k1 = 32 - k;
 804e443:	c7 44 24 24 20 00 00 	movl   $0x20,0x24(%esp)
 804e44a:	00 
 804e44b:	29 74 24 24          	sub    %esi,0x24(%esp)
      z = 0;
 804e44f:	31 f6                	xor    %esi,%esi
 804e451:	8d 76 00             	lea    0x0(%esi),%esi
      do
	{
	  *x1++ = *x << k | z;
 804e454:	8b 1a                	mov    (%edx),%ebx
 804e456:	8a 4c 24 20          	mov    0x20(%esp),%cl
 804e45a:	d3 e3                	shl    %cl,%ebx
 804e45c:	09 f3                	or     %esi,%ebx
 804e45e:	89 18                	mov    %ebx,(%eax)
 804e460:	83 c0 04             	add    $0x4,%eax
	  z = *x++ >> k1;
 804e463:	8b 32                	mov    (%edx),%esi
 804e465:	8a 4c 24 24          	mov    0x24(%esp),%cl
 804e469:	d3 ee                	shr    %cl,%esi
 804e46b:	83 c2 04             	add    $0x4,%edx
	}
      while (x < xe);
 804e46e:	39 54 24 1c          	cmp    %edx,0x1c(%esp)
 804e472:	77 e0                	ja     804e454 <__lshift+0x8c>
      if ((*x1 = z) != 0)
 804e474:	89 30                	mov    %esi,(%eax)
 804e476:	85 f6                	test   %esi,%esi
 804e478:	74 07                	je     804e481 <__lshift+0xb9>
	++n1;
 804e47a:	8b 6c 24 2c          	mov    0x2c(%esp),%ebp
 804e47e:	83 c5 02             	add    $0x2,%ebp
#endif
  else
    do
      *x1++ = *x++;
    while (x < xe);
  b1->_wds = n1 - 1;
 804e481:	8d 45 ff             	lea    -0x1(%ebp),%eax
 804e484:	8b 54 24 28          	mov    0x28(%esp),%edx
 804e488:	89 42 10             	mov    %eax,0x10(%edx)
_DEFUN (Bfree, (ptr, v), struct _reent *ptr _AND _Bigint * v)
{
  _REENT_CHECK_MP(ptr);
  if (v)
    {
      v->_next = _REENT_MP_FREELIST(ptr)[v->_k];
 804e48b:	8b 57 04             	mov    0x4(%edi),%edx
 804e48e:	8b 4c 24 50          	mov    0x50(%esp),%ecx
 804e492:	8b 41 4c             	mov    0x4c(%ecx),%eax
 804e495:	8d 04 90             	lea    (%eax,%edx,4),%eax
 804e498:	8b 10                	mov    (%eax),%edx
 804e49a:	89 17                	mov    %edx,(%edi)
      _REENT_MP_FREELIST(ptr)[v->_k] = v;
 804e49c:	89 38                	mov    %edi,(%eax)
      *x1++ = *x++;
    while (x < xe);
  b1->_wds = n1 - 1;
  Bfree (ptr, b);
  return b1;
}
 804e49e:	8b 44 24 28          	mov    0x28(%esp),%eax
 804e4a2:	83 c4 3c             	add    $0x3c,%esp
 804e4a5:	5b                   	pop    %ebx
 804e4a6:	5e                   	pop    %esi
 804e4a7:	5f                   	pop    %edi
 804e4a8:	5d                   	pop    %ebp
 804e4a9:	c3                   	ret    
 804e4aa:	66 90                	xchg   %ax,%ax
 804e4ac:	8b 5c 24 1c          	mov    0x1c(%esp),%ebx
	++n1;
    }
#endif
  else
    do
      *x1++ = *x++;
 804e4b0:	8b 0a                	mov    (%edx),%ecx
 804e4b2:	89 08                	mov    %ecx,(%eax)
 804e4b4:	83 c0 04             	add    $0x4,%eax
 804e4b7:	83 c2 04             	add    $0x4,%edx
    while (x < xe);
 804e4ba:	39 d3                	cmp    %edx,%ebx
 804e4bc:	77 f2                	ja     804e4b0 <__lshift+0xe8>
 804e4be:	eb c1                	jmp    804e481 <__lshift+0xb9>

0804e4c0 <__mcmp>:
  return b1;
}

int
_DEFUN (cmp, (a, b), _Bigint * a _AND _Bigint * b)
{
 804e4c0:	57                   	push   %edi
 804e4c1:	56                   	push   %esi
 804e4c2:	53                   	push   %ebx
 804e4c3:	8b 4c 24 10          	mov    0x10(%esp),%ecx
 804e4c7:	8b 74 24 14          	mov    0x14(%esp),%esi
  __ULong *xa, *xa0, *xb, *xb0;
  int i, j;

  i = a->_wds;
  j = b->_wds;
 804e4cb:	8b 56 10             	mov    0x10(%esi),%edx
  if (i > 1 && !a->_x[i - 1])
    Bug ("cmp called with a->_x[a->_wds-1] == 0");
  if (j > 1 && !b->_x[j - 1])
    Bug ("cmp called with b->_x[b->_wds-1] == 0");
#endif
  if (i -= j)
 804e4ce:	8b 41 10             	mov    0x10(%ecx),%eax
 804e4d1:	29 d0                	sub    %edx,%eax
 804e4d3:	75 23                	jne    804e4f8 <__mcmp+0x38>
    return i;
  xa0 = a->_x;
 804e4d5:	8d 59 14             	lea    0x14(%ecx),%ebx
  xa = xa0 + j;
 804e4d8:	8d 3c 95 10 00 00 00 	lea    0x10(,%edx,4),%edi
 804e4df:	8d 54 39 04          	lea    0x4(%ecx,%edi,1),%edx
  xb0 = b->_x;
  xb = xb0 + j;
 804e4e3:	8d 4c 3e 04          	lea    0x4(%esi,%edi,1),%ecx
 804e4e7:	90                   	nop
  for (;;)
    {
      if (*--xa != *--xb)
 804e4e8:	83 ea 04             	sub    $0x4,%edx
 804e4eb:	83 e9 04             	sub    $0x4,%ecx
 804e4ee:	8b 31                	mov    (%ecx),%esi
 804e4f0:	39 32                	cmp    %esi,(%edx)
 804e4f2:	75 08                	jne    804e4fc <__mcmp+0x3c>
	return *xa < *xb ? -1 : 1;
      if (xa <= xa0)
 804e4f4:	39 d3                	cmp    %edx,%ebx
 804e4f6:	72 f0                	jb     804e4e8 <__mcmp+0x28>
	break;
    }
  return 0;
}
 804e4f8:	5b                   	pop    %ebx
 804e4f9:	5e                   	pop    %esi
 804e4fa:	5f                   	pop    %edi
 804e4fb:	c3                   	ret    
  xb0 = b->_x;
  xb = xb0 + j;
  for (;;)
    {
      if (*--xa != *--xb)
	return *xa < *xb ? -1 : 1;
 804e4fc:	19 c0                	sbb    %eax,%eax
 804e4fe:	83 c8 01             	or     $0x1,%eax
      if (xa <= xa0)
	break;
    }
  return 0;
}
 804e501:	5b                   	pop    %ebx
 804e502:	5e                   	pop    %esi
 804e503:	5f                   	pop    %edi
 804e504:	c3                   	ret    
 804e505:	8d 76 00             	lea    0x0(%esi),%esi

0804e508 <__mdiff>:

_Bigint *
_DEFUN (diff, (ptr, a, b), struct _reent * ptr _AND
	_Bigint * a _AND _Bigint * b)
{
 804e508:	55                   	push   %ebp
 804e509:	57                   	push   %edi
 804e50a:	56                   	push   %esi
 804e50b:	53                   	push   %ebx
 804e50c:	83 ec 2c             	sub    $0x2c,%esp
 804e50f:	8b 6c 24 40          	mov    0x40(%esp),%ebp
 804e513:	8b 74 24 44          	mov    0x44(%esp),%esi
 804e517:	8b 5c 24 48          	mov    0x48(%esp),%ebx
  __ULong *xa, *xae, *xb, *xbe, *xc;
#ifdef Pack_32
  __Long z;
#endif

  i = cmp (a, b);
 804e51b:	89 5c 24 04          	mov    %ebx,0x4(%esp)
 804e51f:	89 34 24             	mov    %esi,(%esp)
 804e522:	e8 99 ff ff ff       	call   804e4c0 <__mcmp>
  if (!i)
 804e527:	83 f8 00             	cmp    $0x0,%eax
 804e52a:	0f 84 0c 01 00 00    	je     804e63c <__mdiff+0x134>
      c = Balloc (ptr, 0);
      c->_wds = 1;
      c->_x[0] = 0;
      return c;
    }
  if (i < 0)
 804e530:	0f 8c f6 00 00 00    	jl     804e62c <__mdiff+0x124>
      a = b;
      b = c;
      i = 1;
    }
  else
    i = 0;
 804e536:	31 ff                	xor    %edi,%edi
  c = Balloc (ptr, a->_k);
 804e538:	8b 46 04             	mov    0x4(%esi),%eax
 804e53b:	89 44 24 04          	mov    %eax,0x4(%esp)
 804e53f:	89 2c 24             	mov    %ebp,(%esp)
 804e542:	e8 69 f8 ff ff       	call   804ddb0 <_Balloc>
 804e547:	89 44 24 1c          	mov    %eax,0x1c(%esp)
  c->_sign = i;
 804e54b:	89 78 0c             	mov    %edi,0xc(%eax)
  wa = a->_wds;
 804e54e:	8b 7e 10             	mov    0x10(%esi),%edi
  xa = a->_x;
 804e551:	8d 56 14             	lea    0x14(%esi),%edx
  xae = xa + wa;
 804e554:	8d 74 be 14          	lea    0x14(%esi,%edi,4),%esi
 804e558:	89 74 24 14          	mov    %esi,0x14(%esp)
  wb = b->_wds;
  xb = b->_x;
 804e55c:	8d 6b 14             	lea    0x14(%ebx),%ebp
  xbe = xb + wb;
 804e55f:	8b 43 10             	mov    0x10(%ebx),%eax
 804e562:	8d 44 83 14          	lea    0x14(%ebx,%eax,4),%eax
 804e566:	89 44 24 18          	mov    %eax,0x18(%esp)
  xc = c->_x;
 804e56a:	8b 4c 24 1c          	mov    0x1c(%esp),%ecx
 804e56e:	83 c1 14             	add    $0x14,%ecx
  borrow = 0;
 804e571:	31 f6                	xor    %esi,%esi
 804e573:	90                   	nop
#ifdef Pack_32
  do
    {
      y = (*xa & 0xffff) - (*xb & 0xffff) + borrow;
 804e574:	0f b7 1a             	movzwl (%edx),%ebx
 804e577:	01 f3                	add    %esi,%ebx
 804e579:	0f b7 45 00          	movzwl 0x0(%ebp),%eax
 804e57d:	29 c3                	sub    %eax,%ebx
      borrow = y >> 16;
      Sign_Extend (borrow, y);
      z = (*xa++ >> 16) - (*xb++ >> 16) + borrow;
 804e57f:	8b 02                	mov    (%edx),%eax
 804e581:	c1 e8 10             	shr    $0x10,%eax
 804e584:	8b 75 00             	mov    0x0(%ebp),%esi
 804e587:	c1 ee 10             	shr    $0x10,%esi
 804e58a:	29 f0                	sub    %esi,%eax
  borrow = 0;
#ifdef Pack_32
  do
    {
      y = (*xa & 0xffff) - (*xb & 0xffff) + borrow;
      borrow = y >> 16;
 804e58c:	89 de                	mov    %ebx,%esi
 804e58e:	c1 fe 10             	sar    $0x10,%esi
      Sign_Extend (borrow, y);
      z = (*xa++ >> 16) - (*xb++ >> 16) + borrow;
 804e591:	01 f0                	add    %esi,%eax
 804e593:	83 c2 04             	add    $0x4,%edx
 804e596:	83 c5 04             	add    $0x4,%ebp
      borrow = z >> 16;
 804e599:	89 c6                	mov    %eax,%esi
 804e59b:	c1 fe 10             	sar    $0x10,%esi
      Sign_Extend (borrow, z);
      Storeinc (xc, z, y);
 804e59e:	c1 e0 10             	shl    $0x10,%eax
 804e5a1:	81 e3 ff ff 00 00    	and    $0xffff,%ebx
 804e5a7:	09 d8                	or     %ebx,%eax
 804e5a9:	89 01                	mov    %eax,(%ecx)
 804e5ab:	83 c1 04             	add    $0x4,%ecx
    }
  while (xb < xbe);
 804e5ae:	39 6c 24 18          	cmp    %ebp,0x18(%esp)
 804e5b2:	77 c0                	ja     804e574 <__mdiff+0x6c>
      borrow = y >> 16;
      Sign_Extend (borrow, y);
      z = (*xa++ >> 16) - (*xb++ >> 16) + borrow;
      borrow = z >> 16;
      Sign_Extend (borrow, z);
      Storeinc (xc, z, y);
 804e5b4:	89 cd                	mov    %ecx,%ebp
  do
    {
      y = (*xa & 0xffff) - (*xb & 0xffff) + borrow;
      borrow = y >> 16;
      Sign_Extend (borrow, y);
      z = (*xa++ >> 16) - (*xb++ >> 16) + borrow;
 804e5b6:	89 54 24 10          	mov    %edx,0x10(%esp)
      borrow = z >> 16;
      Sign_Extend (borrow, z);
      Storeinc (xc, z, y);
    }
  while (xb < xbe);
  while (xa < xae)
 804e5ba:	39 54 24 14          	cmp    %edx,0x14(%esp)
 804e5be:	76 42                	jbe    804e602 <__mdiff+0xfa>
    {
      y = (*xa & 0xffff) + borrow;
 804e5c0:	8b 02                	mov    (%edx),%eax
 804e5c2:	0f b7 d8             	movzwl %ax,%ebx
 804e5c5:	01 f3                	add    %esi,%ebx
      borrow = y >> 16;
 804e5c7:	89 de                	mov    %ebx,%esi
 804e5c9:	c1 fe 10             	sar    $0x10,%esi
      Sign_Extend (borrow, y);
      z = (*xa++ >> 16) + borrow;
 804e5cc:	c1 e8 10             	shr    $0x10,%eax
 804e5cf:	01 f0                	add    %esi,%eax
 804e5d1:	83 c2 04             	add    $0x4,%edx
      borrow = z >> 16;
 804e5d4:	89 c6                	mov    %eax,%esi
 804e5d6:	c1 fe 10             	sar    $0x10,%esi
      Sign_Extend (borrow, z);
      Storeinc (xc, z, y);
 804e5d9:	c1 e0 10             	shl    $0x10,%eax
 804e5dc:	81 e3 ff ff 00 00    	and    $0xffff,%ebx
 804e5e2:	09 d8                	or     %ebx,%eax
 804e5e4:	89 01                	mov    %eax,(%ecx)
 804e5e6:	83 c1 04             	add    $0x4,%ecx
      borrow = z >> 16;
      Sign_Extend (borrow, z);
      Storeinc (xc, z, y);
    }
  while (xb < xbe);
  while (xa < xae)
 804e5e9:	39 54 24 14          	cmp    %edx,0x14(%esp)
 804e5ed:	77 d1                	ja     804e5c0 <__mdiff+0xb8>
    }
  return 0;
}

_Bigint *
_DEFUN (diff, (ptr, a, b), struct _reent * ptr _AND
 804e5ef:	8b 54 24 10          	mov    0x10(%esp),%edx
 804e5f3:	f7 d2                	not    %edx
 804e5f5:	8b 4c 24 14          	mov    0x14(%esp),%ecx
 804e5f9:	01 ca                	add    %ecx,%edx
 804e5fb:	c1 ea 02             	shr    $0x2,%edx
 804e5fe:	8d 4c 95 04          	lea    0x4(%ebp,%edx,4),%ecx
      borrow = y >> 16;
      Sign_Extend (borrow, y);
      *xc++ = y & 0xffff;
    }
#endif
  while (!*--xc)
 804e602:	85 c0                	test   %eax,%eax
 804e604:	75 13                	jne    804e619 <__mdiff+0x111>
    }
  return 0;
}

_Bigint *
_DEFUN (diff, (ptr, a, b), struct _reent * ptr _AND
 804e606:	89 f8                	mov    %edi,%eax
 804e608:	f7 d8                	neg    %eax
 804e60a:	8d 04 81             	lea    (%ecx,%eax,4),%eax
 804e60d:	8d 76 00             	lea    0x0(%esi),%esi
      Sign_Extend (borrow, y);
      *xc++ = y & 0xffff;
    }
#endif
  while (!*--xc)
    wa--;
 804e610:	4f                   	dec    %edi
      borrow = y >> 16;
      Sign_Extend (borrow, y);
      *xc++ = y & 0xffff;
    }
#endif
  while (!*--xc)
 804e611:	8b 6c b8 fc          	mov    -0x4(%eax,%edi,4),%ebp
 804e615:	85 ed                	test   %ebp,%ebp
 804e617:	74 f7                	je     804e610 <__mdiff+0x108>
    wa--;
  c->_wds = wa;
 804e619:	8b 44 24 1c          	mov    0x1c(%esp),%eax
 804e61d:	89 78 10             	mov    %edi,0x10(%eax)
  return c;
}
 804e620:	8b 44 24 1c          	mov    0x1c(%esp),%eax
 804e624:	83 c4 2c             	add    $0x2c,%esp
 804e627:	5b                   	pop    %ebx
 804e628:	5e                   	pop    %esi
 804e629:	5f                   	pop    %edi
 804e62a:	5d                   	pop    %ebp
 804e62b:	c3                   	ret    
      c = Balloc (ptr, 0);
      c->_wds = 1;
      c->_x[0] = 0;
      return c;
    }
  if (i < 0)
 804e62c:	89 f0                	mov    %esi,%eax
    {
      c = a;
      a = b;
 804e62e:	89 de                	mov    %ebx,%esi
      b = c;
 804e630:	89 c3                	mov    %eax,%ebx
      i = 1;
 804e632:	bf 01 00 00 00       	mov    $0x1,%edi
 804e637:	e9 fc fe ff ff       	jmp    804e538 <__mdiff+0x30>
#endif

  i = cmp (a, b);
  if (!i)
    {
      c = Balloc (ptr, 0);
 804e63c:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
 804e643:	00 
 804e644:	89 2c 24             	mov    %ebp,(%esp)
 804e647:	e8 64 f7 ff ff       	call   804ddb0 <_Balloc>
 804e64c:	89 44 24 1c          	mov    %eax,0x1c(%esp)
      c->_wds = 1;
 804e650:	c7 40 10 01 00 00 00 	movl   $0x1,0x10(%eax)
      c->_x[0] = 0;
 804e657:	c7 40 14 00 00 00 00 	movl   $0x0,0x14(%eax)
#endif
  while (!*--xc)
    wa--;
  c->_wds = wa;
  return c;
}
 804e65e:	8b 44 24 1c          	mov    0x1c(%esp),%eax
 804e662:	83 c4 2c             	add    $0x2c,%esp
 804e665:	5b                   	pop    %ebx
 804e666:	5e                   	pop    %esi
 804e667:	5f                   	pop    %edi
 804e668:	5d                   	pop    %ebp
 804e669:	c3                   	ret    
 804e66a:	66 90                	xchg   %ax,%ax

0804e66c <__ulp>:

double
_DEFUN (ulp, (_x), double _x)
{
 804e66c:	56                   	push   %esi
 804e66d:	53                   	push   %ebx
 804e66e:	83 ec 0c             	sub    $0xc,%esp
  union double_union x, a;
  register __Long L;

  x.d = _x;
 804e671:	8b 54 24 1c          	mov    0x1c(%esp),%edx

  L = (word0 (x) & Exp_mask) - (P - 1) * Exp_msk1;
 804e675:	89 d1                	mov    %edx,%ecx
 804e677:	81 e1 00 00 f0 7f    	and    $0x7ff00000,%ecx
 804e67d:	81 e9 00 00 40 03    	sub    $0x3400000,%ecx
#ifndef Sudden_Underflow
  if (L > 0)
 804e683:	85 c9                	test   %ecx,%ecx
 804e685:	7e 15                	jle    804e69c <__ulp+0x30>
    {
#endif
#ifdef IBM
      L |= Exp_msk1 >> 4;
#endif
      word0 (a) = L;
 804e687:	89 ca                	mov    %ecx,%edx
#ifndef _DOUBLE_IS_32BITS
      word1 (a) = 0;
 804e689:	31 c0                	xor    %eax,%eax
         word1 (a) = L >= 31 ? 1 : 1 << (31 - L);
#endif
	}
    }
#endif
  return a.d;
 804e68b:	89 04 24             	mov    %eax,(%esp)
 804e68e:	89 54 24 04          	mov    %edx,0x4(%esp)
}
 804e692:	dd 04 24             	fldl   (%esp)
 804e695:	83 c4 0c             	add    $0xc,%esp
 804e698:	5b                   	pop    %ebx
 804e699:	5e                   	pop    %esi
 804e69a:	c3                   	ret    
 804e69b:	90                   	nop

#ifndef Sudden_Underflow
    }
  else
    {
      L = -L >> Exp_shift;
 804e69c:	f7 d9                	neg    %ecx
 804e69e:	c1 f9 14             	sar    $0x14,%ecx
      if (L < Exp_shift)
	{
	  word0 (a) = 0x80000 >> L;
 804e6a1:	31 d2                	xor    %edx,%edx
#ifndef Sudden_Underflow
    }
  else
    {
      L = -L >> Exp_shift;
      if (L < Exp_shift)
 804e6a3:	83 f9 13             	cmp    $0x13,%ecx
 804e6a6:	7e 34                	jle    804e6dc <__ulp+0x70>
      else
	{
	  word0 (a) = 0;
	  L -= Exp_shift;
#ifndef _DOUBLE_IS_32BITS
         word1 (a) = L >= 31 ? 1 : 1 << (31 - L);
 804e6a8:	83 f9 32             	cmp    $0x32,%ecx
 804e6ab:	7f 23                	jg     804e6d0 <__ulp+0x64>
 804e6ad:	be 33 00 00 00       	mov    $0x33,%esi
 804e6b2:	29 ce                	sub    %ecx,%esi
 804e6b4:	bb 01 00 00 00       	mov    $0x1,%ebx
 804e6b9:	89 f1                	mov    %esi,%ecx
 804e6bb:	d3 e3                	shl    %cl,%ebx
 804e6bd:	89 d8                	mov    %ebx,%eax
#endif
	}
    }
#endif
  return a.d;
 804e6bf:	89 04 24             	mov    %eax,(%esp)
 804e6c2:	89 54 24 04          	mov    %edx,0x4(%esp)
}
 804e6c6:	dd 04 24             	fldl   (%esp)
 804e6c9:	83 c4 0c             	add    $0xc,%esp
 804e6cc:	5b                   	pop    %ebx
 804e6cd:	5e                   	pop    %esi
 804e6ce:	c3                   	ret    
 804e6cf:	90                   	nop
      else
	{
	  word0 (a) = 0;
	  L -= Exp_shift;
#ifndef _DOUBLE_IS_32BITS
         word1 (a) = L >= 31 ? 1 : 1 << (31 - L);
 804e6d0:	bb 01 00 00 00       	mov    $0x1,%ebx
 804e6d5:	89 d8                	mov    %ebx,%eax
 804e6d7:	eb e6                	jmp    804e6bf <__ulp+0x53>
 804e6d9:	8d 76 00             	lea    0x0(%esi),%esi
  else
    {
      L = -L >> Exp_shift;
      if (L < Exp_shift)
	{
	  word0 (a) = 0x80000 >> L;
 804e6dc:	bb 00 00 08 00       	mov    $0x80000,%ebx
 804e6e1:	89 da                	mov    %ebx,%edx
 804e6e3:	d3 fa                	sar    %cl,%edx
#ifndef _DOUBLE_IS_32BITS
	  word1 (a) = 0;
 804e6e5:	31 c0                	xor    %eax,%eax
         word1 (a) = L >= 31 ? 1 : 1 << (31 - L);
#endif
	}
    }
#endif
  return a.d;
 804e6e7:	89 04 24             	mov    %eax,(%esp)
 804e6ea:	89 54 24 04          	mov    %edx,0x4(%esp)
}
 804e6ee:	dd 04 24             	fldl   (%esp)
 804e6f1:	83 c4 0c             	add    $0xc,%esp
 804e6f4:	5b                   	pop    %ebx
 804e6f5:	5e                   	pop    %esi
 804e6f6:	c3                   	ret    
 804e6f7:	90                   	nop

0804e6f8 <__b2d>:

double
_DEFUN (b2d, (a, e),
	_Bigint * a _AND int *e)
{
 804e6f8:	55                   	push   %ebp
 804e6f9:	57                   	push   %edi
 804e6fa:	56                   	push   %esi
 804e6fb:	53                   	push   %ebx
 804e6fc:	83 ec 18             	sub    $0x18,%esp
 804e6ff:	8b 44 24 2c          	mov    0x2c(%esp),%eax
#else
#define d0 word0(d)
#define d1 word1(d)
#endif

  xa0 = a->_x;
 804e703:	8d 50 14             	lea    0x14(%eax),%edx
 804e706:	89 54 24 10          	mov    %edx,0x10(%esp)
  xa = xa0 + a->_wds;
 804e70a:	8b 50 10             	mov    0x10(%eax),%edx
 804e70d:	8d 74 90 10          	lea    0x10(%eax,%edx,4),%esi
 804e711:	8d 6e 04             	lea    0x4(%esi),%ebp
  y = *--xa;
 804e714:	8b 5d fc             	mov    -0x4(%ebp),%ebx
#ifdef DEBUG
  if (!y)
    Bug ("zero y in b2d");
#endif
  k = hi0bits (y);
 804e717:	89 1c 24             	mov    %ebx,(%esp)
 804e71a:	e8 11 f9 ff ff       	call   804e030 <__hi0bits>
  *e = 32 - k;
 804e71f:	ba 20 00 00 00       	mov    $0x20,%edx
 804e724:	29 c2                	sub    %eax,%edx
 804e726:	8b 4c 24 30          	mov    0x30(%esp),%ecx
 804e72a:	89 11                	mov    %edx,(%ecx)
#ifdef Pack_32
  if (k < Ebits)
 804e72c:	83 f8 0a             	cmp    $0xa,%eax
 804e72f:	7f 43                	jg     804e774 <__b2d+0x7c>
    {
      d0 = Exp_1 | y >> (Ebits - k);
 804e731:	b9 0b 00 00 00       	mov    $0xb,%ecx
 804e736:	29 c1                	sub    %eax,%ecx
 804e738:	89 da                	mov    %ebx,%edx
 804e73a:	d3 ea                	shr    %cl,%edx
 804e73c:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
 804e743:	00 
 804e744:	81 ca 00 00 f0 3f    	or     $0x3ff00000,%edx
 804e74a:	89 54 24 08          	mov    %edx,0x8(%esp)
      w = xa > xa0 ? *--xa : 0;
 804e74e:	39 74 24 10          	cmp    %esi,0x10(%esp)
 804e752:	0f 83 9c 00 00 00    	jae    804e7f4 <__b2d+0xfc>
 804e758:	8b 55 f8             	mov    -0x8(%ebp),%edx
 804e75b:	d3 ea                	shr    %cl,%edx
#ifndef _DOUBLE_IS_32BITS
      d1 = y << ((32 - Ebits) + k) | w >> (Ebits - k);
 804e75d:	8d 48 15             	lea    0x15(%eax),%ecx
 804e760:	d3 e3                	shl    %cl,%ebx
 804e762:	09 da                	or     %ebx,%edx
 804e764:	89 54 24 04          	mov    %edx,0x4(%esp)
  word1 (d) = d1 >> 16 | d1 << 16;
#else
#undef d0
#undef d1
#endif
  return d.d;
 804e768:	dd 44 24 04          	fldl   0x4(%esp)
}
 804e76c:	83 c4 18             	add    $0x18,%esp
 804e76f:	5b                   	pop    %ebx
 804e770:	5e                   	pop    %esi
 804e771:	5f                   	pop    %edi
 804e772:	5d                   	pop    %ebp
 804e773:	c3                   	ret    
 804e774:	89 f7                	mov    %esi,%edi
#ifndef _DOUBLE_IS_32BITS
      d1 = y << ((32 - Ebits) + k) | w >> (Ebits - k);
#endif
      goto ret_d;
    }
  z = xa > xa0 ? *--xa : 0;
 804e776:	39 74 24 10          	cmp    %esi,0x10(%esp)
 804e77a:	72 50                	jb     804e7cc <__b2d+0xd4>
 804e77c:	31 d2                	xor    %edx,%edx
  if (k -= Ebits)
 804e77e:	89 c5                	mov    %eax,%ebp
 804e780:	83 ed 0b             	sub    $0xb,%ebp
 804e783:	74 54                	je     804e7d9 <__b2d+0xe1>
    {
      d0 = Exp_1 | y << k | z >> (32 - k);
 804e785:	be 2b 00 00 00       	mov    $0x2b,%esi
 804e78a:	29 c6                	sub    %eax,%esi
 804e78c:	89 e9                	mov    %ebp,%ecx
 804e78e:	d3 e3                	shl    %cl,%ebx
 804e790:	81 cb 00 00 f0 3f    	or     $0x3ff00000,%ebx
 804e796:	89 d0                	mov    %edx,%eax
 804e798:	89 f1                	mov    %esi,%ecx
 804e79a:	d3 e8                	shr    %cl,%eax
 804e79c:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
 804e7a3:	00 
 804e7a4:	09 c3                	or     %eax,%ebx
 804e7a6:	89 5c 24 08          	mov    %ebx,0x8(%esp)
      y = xa > xa0 ? *--xa : 0;
 804e7aa:	3b 7c 24 10          	cmp    0x10(%esp),%edi
 804e7ae:	76 4c                	jbe    804e7fc <__b2d+0x104>
 804e7b0:	8b 47 fc             	mov    -0x4(%edi),%eax
 804e7b3:	d3 e8                	shr    %cl,%eax
#ifndef _DOUBLE_IS_32BITS
      d1 = z << k | y >> (32 - k);
 804e7b5:	89 e9                	mov    %ebp,%ecx
 804e7b7:	d3 e2                	shl    %cl,%edx
 804e7b9:	09 d0                	or     %edx,%eax
 804e7bb:	89 44 24 04          	mov    %eax,0x4(%esp)
  word1 (d) = d1 >> 16 | d1 << 16;
#else
#undef d0
#undef d1
#endif
  return d.d;
 804e7bf:	dd 44 24 04          	fldl   0x4(%esp)
}
 804e7c3:	83 c4 18             	add    $0x18,%esp
 804e7c6:	5b                   	pop    %ebx
 804e7c7:	5e                   	pop    %esi
 804e7c8:	5f                   	pop    %edi
 804e7c9:	5d                   	pop    %ebp
 804e7ca:	c3                   	ret    
 804e7cb:	90                   	nop
#ifndef _DOUBLE_IS_32BITS
      d1 = y << ((32 - Ebits) + k) | w >> (Ebits - k);
#endif
      goto ret_d;
    }
  z = xa > xa0 ? *--xa : 0;
 804e7cc:	83 ef 04             	sub    $0x4,%edi
 804e7cf:	8b 55 f8             	mov    -0x8(%ebp),%edx
  if (k -= Ebits)
 804e7d2:	89 c5                	mov    %eax,%ebp
 804e7d4:	83 ed 0b             	sub    $0xb,%ebp
 804e7d7:	75 ac                	jne    804e785 <__b2d+0x8d>
      d1 = z << k | y >> (32 - k);
#endif
    }
  else
    {
      d0 = Exp_1 | y;
 804e7d9:	81 cb 00 00 f0 3f    	or     $0x3ff00000,%ebx
 804e7df:	89 5c 24 08          	mov    %ebx,0x8(%esp)
#ifndef _DOUBLE_IS_32BITS
      d1 = z;
 804e7e3:	89 54 24 04          	mov    %edx,0x4(%esp)
  word1 (d) = d1 >> 16 | d1 << 16;
#else
#undef d0
#undef d1
#endif
  return d.d;
 804e7e7:	dd 44 24 04          	fldl   0x4(%esp)
}
 804e7eb:	83 c4 18             	add    $0x18,%esp
 804e7ee:	5b                   	pop    %ebx
 804e7ef:	5e                   	pop    %esi
 804e7f0:	5f                   	pop    %edi
 804e7f1:	5d                   	pop    %ebp
 804e7f2:	c3                   	ret    
 804e7f3:	90                   	nop
  *e = 32 - k;
#ifdef Pack_32
  if (k < Ebits)
    {
      d0 = Exp_1 | y >> (Ebits - k);
      w = xa > xa0 ? *--xa : 0;
 804e7f4:	31 d2                	xor    %edx,%edx
 804e7f6:	e9 62 ff ff ff       	jmp    804e75d <__b2d+0x65>
 804e7fb:	90                   	nop
    }
  z = xa > xa0 ? *--xa : 0;
  if (k -= Ebits)
    {
      d0 = Exp_1 | y << k | z >> (32 - k);
      y = xa > xa0 ? *--xa : 0;
 804e7fc:	31 c0                	xor    %eax,%eax
 804e7fe:	eb b5                	jmp    804e7b5 <__b2d+0xbd>

0804e800 <__d2b>:
	struct _reent * ptr _AND
	double _d _AND
	int *e _AND
	int *bits)

{
 804e800:	55                   	push   %ebp
 804e801:	57                   	push   %edi
 804e802:	56                   	push   %esi
 804e803:	53                   	push   %ebx
 804e804:	83 ec 2c             	sub    $0x2c,%esp
  int de, i, k;
  __ULong *x, y, z;
#ifdef VAX
  __ULong d0, d1;
#endif
  d.d = _d;
 804e807:	8b 74 24 44          	mov    0x44(%esp),%esi
 804e80b:	8b 7c 24 48          	mov    0x48(%esp),%edi
#define d1 word1(d)
  d.d = _d;
#endif

#ifdef Pack_32
  b = Balloc (ptr, 1);
 804e80f:	c7 44 24 04 01 00 00 	movl   $0x1,0x4(%esp)
 804e816:	00 
 804e817:	8b 44 24 40          	mov    0x40(%esp),%eax
 804e81b:	89 04 24             	mov    %eax,(%esp)
 804e81e:	e8 8d f5 ff ff       	call   804ddb0 <_Balloc>
 804e823:	89 c5                	mov    %eax,%ebp
#else
  b = Balloc (ptr, 2);
#endif
  x = b->_x;

  z = d0 & Frac_mask;
 804e825:	89 fb                	mov    %edi,%ebx
 804e827:	89 f8                	mov    %edi,%eax
 804e829:	25 ff ff 0f 00       	and    $0xfffff,%eax
  d0 &= 0x7fffffff;		/* clear sign bit, which we ignore */
 804e82e:	81 e3 ff ff ff 7f    	and    $0x7fffffff,%ebx
  de = (int) (d0 >> Exp_shift);
#ifndef IBM
  z |= Exp_msk11;
#endif
#else
  if ((de = (int) (d0 >> Exp_shift)) != 0)
 804e834:	c1 eb 14             	shr    $0x14,%ebx
 804e837:	74 05                	je     804e83e <__d2b+0x3e>
    z |= Exp_msk1;
 804e839:	0d 00 00 10 00       	or     $0x100000,%eax
 804e83e:	89 44 24 1c          	mov    %eax,0x1c(%esp)
#endif
#ifdef Pack_32
#ifndef _DOUBLE_IS_32BITS
  if (d1)
 804e842:	85 f6                	test   %esi,%esi
 804e844:	74 5a                	je     804e8a0 <__d2b+0xa0>
    {
      y = d1;
 804e846:	89 74 24 18          	mov    %esi,0x18(%esp)
      k = lo0bits (&y);
 804e84a:	8d 44 24 18          	lea    0x18(%esp),%eax
 804e84e:	89 04 24             	mov    %eax,(%esp)
 804e851:	e8 32 f8 ff ff       	call   804e088 <__lo0bits>
      if (k)
 804e856:	85 c0                	test   %eax,%eax
 804e858:	0f 85 96 00 00 00    	jne    804e8f4 <__d2b+0xf4>
	{
         x[0] = y | z << (32 - k);
	  z >>= k;
	}
      else
	x[0] = y;
 804e85e:	8b 54 24 18          	mov    0x18(%esp),%edx
 804e862:	89 55 14             	mov    %edx,0x14(%ebp)
 804e865:	8b 54 24 1c          	mov    0x1c(%esp),%edx
      i = b->_wds = (x[1] = z) ? 2 : 1;
 804e869:	89 55 18             	mov    %edx,0x18(%ebp)
 804e86c:	83 fa 01             	cmp    $0x1,%edx
 804e86f:	19 f6                	sbb    %esi,%esi
 804e871:	83 c6 02             	add    $0x2,%esi
 804e874:	89 75 10             	mov    %esi,0x10(%ebp)
  while (!x[i])
    --i;
  b->_wds = i + 1;
#endif
#ifndef Sudden_Underflow
  if (de)
 804e877:	85 db                	test   %ebx,%ebx
 804e879:	74 4b                	je     804e8c6 <__d2b+0xc6>
#endif
#ifdef IBM
      *e = (de - Bias - (P - 1) << 2) + k;
      *bits = 4 * P + 8 - k - hi0bits (word0 (d) & Frac_mask);
#else
      *e = de - Bias - (P - 1) + k;
 804e87b:	8d 94 18 cd fb ff ff 	lea    -0x433(%eax,%ebx,1),%edx
 804e882:	8b 74 24 4c          	mov    0x4c(%esp),%esi
 804e886:	89 16                	mov    %edx,(%esi)
      *bits = P - k;
 804e888:	ba 35 00 00 00       	mov    $0x35,%edx
 804e88d:	29 c2                	sub    %eax,%edx
 804e88f:	8b 74 24 50          	mov    0x50(%esp),%esi
 804e893:	89 16                	mov    %edx,(%esi)
      *bits = (i + 2) * 16 - hi0bits (x[i]);
#endif
    }
#endif
  return b;
}
 804e895:	89 e8                	mov    %ebp,%eax
 804e897:	83 c4 2c             	add    $0x2c,%esp
 804e89a:	5b                   	pop    %ebx
 804e89b:	5e                   	pop    %esi
 804e89c:	5f                   	pop    %edi
 804e89d:	5d                   	pop    %ebp
 804e89e:	c3                   	ret    
 804e89f:	90                   	nop
    {
#ifdef DEBUG
      if (!z)
	Bug ("Zero passed to d2b");
#endif
      k = lo0bits (&z);
 804e8a0:	8d 44 24 1c          	lea    0x1c(%esp),%eax
 804e8a4:	89 04 24             	mov    %eax,(%esp)
 804e8a7:	e8 dc f7 ff ff       	call   804e088 <__lo0bits>
      x[0] = z;
 804e8ac:	8b 54 24 1c          	mov    0x1c(%esp),%edx
 804e8b0:	89 55 14             	mov    %edx,0x14(%ebp)
      i = b->_wds = 1;
 804e8b3:	c7 45 10 01 00 00 00 	movl   $0x1,0x10(%ebp)
#ifndef _DOUBLE_IS_32BITS
      k += 32;
 804e8ba:	83 c0 20             	add    $0x20,%eax
      if (!z)
	Bug ("Zero passed to d2b");
#endif
      k = lo0bits (&z);
      x[0] = z;
      i = b->_wds = 1;
 804e8bd:	be 01 00 00 00       	mov    $0x1,%esi
  while (!x[i])
    --i;
  b->_wds = i + 1;
#endif
#ifndef Sudden_Underflow
  if (de)
 804e8c2:	85 db                	test   %ebx,%ebx
 804e8c4:	75 b5                	jne    804e87b <__d2b+0x7b>
#endif
#ifndef Sudden_Underflow
    }
  else
    {
      *e = de - Bias - (P - 1) + 1 + k;
 804e8c6:	2d 32 04 00 00       	sub    $0x432,%eax
 804e8cb:	8b 54 24 4c          	mov    0x4c(%esp),%edx
 804e8cf:	89 02                	mov    %eax,(%edx)
#ifdef Pack_32
      *bits = 32 * i - hi0bits (x[i - 1]);
 804e8d1:	8b 44 b5 10          	mov    0x10(%ebp,%esi,4),%eax
 804e8d5:	89 04 24             	mov    %eax,(%esp)
 804e8d8:	e8 53 f7 ff ff       	call   804e030 <__hi0bits>
 804e8dd:	c1 e6 05             	shl    $0x5,%esi
 804e8e0:	29 c6                	sub    %eax,%esi
 804e8e2:	8b 54 24 50          	mov    0x50(%esp),%edx
 804e8e6:	89 32                	mov    %esi,(%edx)
      *bits = (i + 2) * 16 - hi0bits (x[i]);
#endif
    }
#endif
  return b;
}
 804e8e8:	89 e8                	mov    %ebp,%eax
 804e8ea:	83 c4 2c             	add    $0x2c,%esp
 804e8ed:	5b                   	pop    %ebx
 804e8ee:	5e                   	pop    %esi
 804e8ef:	5f                   	pop    %edi
 804e8f0:	5d                   	pop    %ebp
 804e8f1:	c3                   	ret    
 804e8f2:	66 90                	xchg   %ax,%ax
    {
      y = d1;
      k = lo0bits (&y);
      if (k)
	{
         x[0] = y | z << (32 - k);
 804e8f4:	8b 54 24 1c          	mov    0x1c(%esp),%edx
 804e8f8:	b9 20 00 00 00       	mov    $0x20,%ecx
 804e8fd:	29 c1                	sub    %eax,%ecx
 804e8ff:	89 d6                	mov    %edx,%esi
 804e901:	d3 e6                	shl    %cl,%esi
 804e903:	8b 4c 24 18          	mov    0x18(%esp),%ecx
 804e907:	09 f1                	or     %esi,%ecx
 804e909:	89 4d 14             	mov    %ecx,0x14(%ebp)
	  z >>= k;
 804e90c:	88 c1                	mov    %al,%cl
 804e90e:	d3 ea                	shr    %cl,%edx
 804e910:	89 54 24 1c          	mov    %edx,0x1c(%esp)
 804e914:	e9 50 ff ff ff       	jmp    804e869 <__d2b+0x69>
 804e919:	8d 76 00             	lea    0x0(%esi),%esi

0804e91c <__ratio>:
#undef d1

double
_DEFUN (ratio, (a, b), _Bigint * a _AND _Bigint * b)

{
 804e91c:	55                   	push   %ebp
 804e91d:	57                   	push   %edi
 804e91e:	56                   	push   %esi
 804e91f:	53                   	push   %ebx
 804e920:	83 ec 2c             	sub    $0x2c,%esp
 804e923:	8b 6c 24 40          	mov    0x40(%esp),%ebp
 804e927:	8b 5c 24 44          	mov    0x44(%esp),%ebx
  union double_union da, db;
  int k, ka, kb;

  da.d = b2d (a, &ka);
 804e92b:	8d 44 24 20          	lea    0x20(%esp),%eax
 804e92f:	89 44 24 04          	mov    %eax,0x4(%esp)
 804e933:	89 2c 24             	mov    %ebp,(%esp)
 804e936:	e8 bd fd ff ff       	call   804e6f8 <__b2d>
 804e93b:	dd 5c 24 10          	fstpl  0x10(%esp)
 804e93f:	8b 74 24 10          	mov    0x10(%esp),%esi
 804e943:	8b 7c 24 14          	mov    0x14(%esp),%edi
  db.d = b2d (b, &kb);
 804e947:	8d 44 24 24          	lea    0x24(%esp),%eax
 804e94b:	89 44 24 04          	mov    %eax,0x4(%esp)
 804e94f:	89 1c 24             	mov    %ebx,(%esp)
 804e952:	e8 a1 fd ff ff       	call   804e6f8 <__b2d>
 804e957:	dd 54 24 08          	fstl   0x8(%esp)
 804e95b:	8b 44 24 08          	mov    0x8(%esp),%eax
 804e95f:	8b 54 24 0c          	mov    0xc(%esp),%edx
#ifdef Pack_32
  k = ka - kb + 32 * (a->_wds - b->_wds);
 804e963:	8b 4c 24 20          	mov    0x20(%esp),%ecx
 804e967:	2b 4c 24 24          	sub    0x24(%esp),%ecx
 804e96b:	8b 6d 10             	mov    0x10(%ebp),%ebp
 804e96e:	2b 6b 10             	sub    0x10(%ebx),%ebp
 804e971:	c1 e5 05             	shl    $0x5,%ebp
 804e974:	01 e9                	add    %ebp,%ecx
      word0 (db) += (k >> 2) * Exp_msk1;
      if (k &= 3)
	db.d *= 1 << k;
    }
#else
  if (k > 0)
 804e976:	85 c9                	test   %ecx,%ecx
 804e978:	7e 1a                	jle    804e994 <__ratio+0x78>
    word0 (da) += k * Exp_msk1;
 804e97a:	c1 e1 14             	shl    $0x14,%ecx
 804e97d:	01 cf                	add    %ecx,%edi
 804e97f:	89 74 24 10          	mov    %esi,0x10(%esp)
 804e983:	89 7c 24 14          	mov    %edi,0x14(%esp)
    {
      k = -k;
      word0 (db) += k * Exp_msk1;
    }
#endif
  return da.d / db.d;
 804e987:	dc 7c 24 10          	fdivrl 0x10(%esp)
}
 804e98b:	83 c4 2c             	add    $0x2c,%esp
 804e98e:	5b                   	pop    %ebx
 804e98f:	5e                   	pop    %esi
 804e990:	5f                   	pop    %edi
 804e991:	5d                   	pop    %ebp
 804e992:	c3                   	ret    
 804e993:	90                   	nop
 804e994:	dd d8                	fstp   %st(0)
  if (k > 0)
    word0 (da) += k * Exp_msk1;
  else
    {
      k = -k;
      word0 (db) += k * Exp_msk1;
 804e996:	c1 e1 14             	shl    $0x14,%ecx
 804e999:	29 ca                	sub    %ecx,%edx
 804e99b:	89 44 24 08          	mov    %eax,0x8(%esp)
 804e99f:	89 54 24 0c          	mov    %edx,0xc(%esp)
 804e9a3:	dd 44 24 08          	fldl   0x8(%esp)
    }
#endif
  return da.d / db.d;
 804e9a7:	dc 7c 24 10          	fdivrl 0x10(%esp)
}
 804e9ab:	83 c4 2c             	add    $0x2c,%esp
 804e9ae:	5b                   	pop    %ebx
 804e9af:	5e                   	pop    %esi
 804e9b0:	5f                   	pop    %edi
 804e9b1:	5d                   	pop    %ebp
 804e9b2:	c3                   	ret    
 804e9b3:	90                   	nop

0804e9b4 <_mprec_log10>:


double
_DEFUN (_mprec_log10, (dig),
	int dig)
{
 804e9b4:	8b 44 24 04          	mov    0x4(%esp),%eax
  double v = 1.0;
  if (dig < 24)
 804e9b8:	83 f8 17             	cmp    $0x17,%eax
 804e9bb:	7e 13                	jle    804e9d0 <_mprec_log10+0x1c>
 804e9bd:	d9 e8                	fld1   
    return tens[dig];
  while (dig > 0)
    {
      v *= 10;
 804e9bf:	d9 05 6c 18 05 08    	flds   0x805186c
 804e9c5:	8d 76 00             	lea    0x0(%esi),%esi
 804e9c8:	dc c9                	fmul   %st,%st(1)
	int dig)
{
  double v = 1.0;
  if (dig < 24)
    return tens[dig];
  while (dig > 0)
 804e9ca:	48                   	dec    %eax
 804e9cb:	75 fb                	jne    804e9c8 <_mprec_log10+0x14>
 804e9cd:	dd d8                	fstp   %st(0)
    {
      v *= 10;
      dig--;
    }
  return v;
}
 804e9cf:	c3                   	ret    
_DEFUN (_mprec_log10, (dig),
	int dig)
{
  double v = 1.0;
  if (dig < 24)
    return tens[dig];
 804e9d0:	dd 04 c5 20 19 05 08 	fldl   0x8051920(,%eax,8)
 804e9d7:	c3                   	ret    

0804e9d8 <__copybits>:
void
_DEFUN (copybits, (c, n, b),
	__ULong *c _AND
	int n _AND
	_Bigint *b)
{
 804e9d8:	55                   	push   %ebp
 804e9d9:	57                   	push   %edi
 804e9da:	56                   	push   %esi
 804e9db:	53                   	push   %ebx
 804e9dc:	8b 44 24 14          	mov    0x14(%esp),%eax
 804e9e0:	8b 7c 24 1c          	mov    0x1c(%esp),%edi
	__ULong *ce, *x, *xe;
#ifdef Pack_16
	int nw, nw1;
#endif

	ce = c + ((n-1) >> kshift) + 1;
 804e9e4:	8b 54 24 18          	mov    0x18(%esp),%edx
 804e9e8:	4a                   	dec    %edx
 804e9e9:	c1 fa 05             	sar    $0x5,%edx
 804e9ec:	8d 74 90 04          	lea    0x4(%eax,%edx,4),%esi
	x = b->_x;
 804e9f0:	8d 57 14             	lea    0x14(%edi),%edx
#ifdef Pack_32
	xe = x + b->_wds;
 804e9f3:	8b 4f 10             	mov    0x10(%edi),%ecx
 804e9f6:	8d 5c 8f 14          	lea    0x14(%edi,%ecx,4),%ebx
	while(x < xe)
 804e9fa:	39 da                	cmp    %ebx,%edx
 804e9fc:	73 2b                	jae    804ea29 <__copybits+0x51>
 804e9fe:	89 c1                	mov    %eax,%ecx
		*c++ = *x++;
 804ea00:	8b 2a                	mov    (%edx),%ebp
 804ea02:	89 29                	mov    %ebp,(%ecx)
 804ea04:	83 c1 04             	add    $0x4,%ecx
 804ea07:	83 c2 04             	add    $0x4,%edx

	ce = c + ((n-1) >> kshift) + 1;
	x = b->_x;
#ifdef Pack_32
	xe = x + b->_wds;
	while(x < xe)
 804ea0a:	39 d3                	cmp    %edx,%ebx
 804ea0c:	77 f2                	ja     804ea00 <__copybits+0x28>
    }
  return v;
}

void
_DEFUN (copybits, (c, n, b),
 804ea0e:	29 fb                	sub    %edi,%ebx
 804ea10:	8d 53 eb             	lea    -0x15(%ebx),%edx
 804ea13:	c1 ea 02             	shr    $0x2,%edx
 804ea16:	8d 44 90 04          	lea    0x4(%eax,%edx,4),%eax
	for(xe = x + (nw - nw1); x < xe; x += 2)
		Storeinc(c, x[1], x[0]);
	if (nw1)
		*c++ = *x;
#endif
	while(c < ce)
 804ea1a:	39 c6                	cmp    %eax,%esi
 804ea1c:	76 0f                	jbe    804ea2d <__copybits+0x55>
 804ea1e:	66 90                	xchg   %ax,%ax
		*c++ = 0;
 804ea20:	c7 00 00 00 00 00    	movl   $0x0,(%eax)
 804ea26:	83 c0 04             	add    $0x4,%eax
	for(xe = x + (nw - nw1); x < xe; x += 2)
		Storeinc(c, x[1], x[0]);
	if (nw1)
		*c++ = *x;
#endif
	while(c < ce)
 804ea29:	39 c6                	cmp    %eax,%esi
 804ea2b:	77 f3                	ja     804ea20 <__copybits+0x48>
		*c++ = 0;
}
 804ea2d:	5b                   	pop    %ebx
 804ea2e:	5e                   	pop    %esi
 804ea2f:	5f                   	pop    %edi
 804ea30:	5d                   	pop    %ebp
 804ea31:	c3                   	ret    
 804ea32:	66 90                	xchg   %ax,%ax

0804ea34 <__any_on>:

__ULong
_DEFUN (any_on, (b, k),
	_Bigint *b _AND
	int k)
{
 804ea34:	56                   	push   %esi
 804ea35:	53                   	push   %ebx
 804ea36:	8b 44 24 0c          	mov    0xc(%esp),%eax
	int n, nwds;
	__ULong *x, *x0, x1, x2;

	x = b->_x;
 804ea3a:	8d 50 14             	lea    0x14(%eax),%edx
	nwds = b->_wds;
 804ea3d:	8b 48 10             	mov    0x10(%eax),%ecx
	n = k >> kshift;
 804ea40:	8b 5c 24 10          	mov    0x10(%esp),%ebx
 804ea44:	c1 fb 05             	sar    $0x5,%ebx
	if (n > nwds)
 804ea47:	39 d9                	cmp    %ebx,%ecx
 804ea49:	7d 29                	jge    804ea74 <__any_on+0x40>
 804ea4b:	89 cb                	mov    %ecx,%ebx
		x1 <<= k;
		if (x1 != x2)
			return 1;
		}
	x0 = x;
	x += n;
 804ea4d:	8d 44 98 10          	lea    0x10(%eax,%ebx,4),%eax
 804ea51:	8d 48 04             	lea    0x4(%eax),%ecx
	while(x > x0)
 804ea54:	39 ca                	cmp    %ecx,%edx
 804ea56:	72 0b                	jb     804ea63 <__any_on+0x2f>
 804ea58:	eb 36                	jmp    804ea90 <__any_on+0x5c>
 804ea5a:	66 90                	xchg   %ax,%ax
 804ea5c:	39 c2                	cmp    %eax,%edx
 804ea5e:	73 30                	jae    804ea90 <__any_on+0x5c>
		if (*--x)
 804ea60:	83 e8 04             	sub    $0x4,%eax
 804ea63:	8b 08                	mov    (%eax),%ecx
 804ea65:	85 c9                	test   %ecx,%ecx
 804ea67:	74 f3                	je     804ea5c <__any_on+0x28>
	else if (n < nwds && (k &= kmask)) {
		x1 = x2 = x[n];
		x1 >>= k;
		x1 <<= k;
		if (x1 != x2)
			return 1;
 804ea69:	b8 01 00 00 00       	mov    $0x1,%eax
	x += n;
	while(x > x0)
		if (*--x)
			return 1;
	return 0;
}
 804ea6e:	5b                   	pop    %ebx
 804ea6f:	5e                   	pop    %esi
 804ea70:	c3                   	ret    
 804ea71:	8d 76 00             	lea    0x0(%esi),%esi
	x = b->_x;
	nwds = b->_wds;
	n = k >> kshift;
	if (n > nwds)
		n = nwds;
	else if (n < nwds && (k &= kmask)) {
 804ea74:	7e d7                	jle    804ea4d <__any_on+0x19>
 804ea76:	8b 4c 24 10          	mov    0x10(%esp),%ecx
 804ea7a:	83 e1 1f             	and    $0x1f,%ecx
 804ea7d:	74 ce                	je     804ea4d <__any_on+0x19>
		x1 = x2 = x[n];
		x1 >>= k;
 804ea7f:	8b 74 98 14          	mov    0x14(%eax,%ebx,4),%esi
 804ea83:	d3 ee                	shr    %cl,%esi
		x1 <<= k;
 804ea85:	d3 e6                	shl    %cl,%esi
		if (x1 != x2)
 804ea87:	3b 74 98 14          	cmp    0x14(%eax,%ebx,4),%esi
 804ea8b:	75 dc                	jne    804ea69 <__any_on+0x35>
 804ea8d:	eb be                	jmp    804ea4d <__any_on+0x19>
 804ea8f:	90                   	nop
	x0 = x;
	x += n;
	while(x > x0)
		if (*--x)
			return 1;
	return 0;
 804ea90:	31 c0                	xor    %eax,%eax
}
 804ea92:	5b                   	pop    %ebx
 804ea93:	5e                   	pop    %esi
 804ea94:	c3                   	ret    
 804ea95:	90                   	nop
 804ea96:	90                   	nop
 804ea97:	90                   	nop

0804ea98 <nk_do_syscall>:
#include <stdint.h>
#include <errno.h>
#include "posnk_sc.h"

uint32_t __attribute__((optimize("O0"))) nk_do_syscall(uint32_t no, uint32_t param[4], uint32_t param_size[4])
{
 804ea98:	83 ec 4c             	sub    $0x4c,%esp
	int i;
	volatile syscall_params_t p;
	uint32_t pa = (uint32_t) &p;
 804ea9b:	8d 44 24 08          	lea    0x8(%esp),%eax
 804ea9f:	89 44 24 38          	mov    %eax,0x38(%esp)
	p.magic = SYSCALL_MAGIC;
 804eaa3:	c7 44 24 08 11 ca fe 	movl   $0xcafeca11,0x8(%esp)
 804eaaa:	ca 
	p.call_id = no;
 804eaab:	8b 44 24 50          	mov    0x50(%esp),%eax
 804eaaf:	89 44 24 0c          	mov    %eax,0xc(%esp)
	for (i = 0; i < 4; i++)
 804eab3:	c7 44 24 3c 00 00 00 	movl   $0x0,0x3c(%esp)
 804eaba:	00 
 804eabb:	eb 1f                	jmp    804eadc <nk_do_syscall+0x44>
		p.param[i] = param[i];
 804eabd:	8b 44 24 3c          	mov    0x3c(%esp),%eax
 804eac1:	8d 14 85 00 00 00 00 	lea    0x0(,%eax,4),%edx
 804eac8:	8b 44 24 54          	mov    0x54(%esp),%eax
 804eacc:	01 d0                	add    %edx,%eax
 804eace:	8b 10                	mov    (%eax),%edx
 804ead0:	8b 44 24 3c          	mov    0x3c(%esp),%eax
 804ead4:	89 54 84 10          	mov    %edx,0x10(%esp,%eax,4)
	int i;
	volatile syscall_params_t p;
	uint32_t pa = (uint32_t) &p;
	p.magic = SYSCALL_MAGIC;
	p.call_id = no;
	for (i = 0; i < 4; i++)
 804ead8:	ff 44 24 3c          	incl   0x3c(%esp)
 804eadc:	83 7c 24 3c 03       	cmpl   $0x3,0x3c(%esp)
 804eae1:	7e da                	jle    804eabd <nk_do_syscall+0x25>
		p.param[i] = param[i];
	for (i = 0; i < 4; i++)
 804eae3:	c7 44 24 3c 00 00 00 	movl   $0x0,0x3c(%esp)
 804eaea:	00 
 804eaeb:	eb 22                	jmp    804eb0f <nk_do_syscall+0x77>
		p.param_size[i] = param_size[i];
 804eaed:	8b 44 24 3c          	mov    0x3c(%esp),%eax
 804eaf1:	8d 14 85 00 00 00 00 	lea    0x0(,%eax,4),%edx
 804eaf8:	8b 44 24 58          	mov    0x58(%esp),%eax
 804eafc:	01 d0                	add    %edx,%eax
 804eafe:	8b 00                	mov    (%eax),%eax
 804eb00:	8b 54 24 3c          	mov    0x3c(%esp),%edx
 804eb04:	83 c2 04             	add    $0x4,%edx
 804eb07:	89 44 94 10          	mov    %eax,0x10(%esp,%edx,4)
	uint32_t pa = (uint32_t) &p;
	p.magic = SYSCALL_MAGIC;
	p.call_id = no;
	for (i = 0; i < 4; i++)
		p.param[i] = param[i];
	for (i = 0; i < 4; i++)
 804eb0b:	ff 44 24 3c          	incl   0x3c(%esp)
 804eb0f:	83 7c 24 3c 03       	cmpl   $0x3,0x3c(%esp)
 804eb14:	7e d7                	jle    804eaed <nk_do_syscall+0x55>
		p.param_size[i] = param_size[i];
	asm ("movl %0, %%eax;int $0x80;"::"r"(pa));
 804eb16:	8b 44 24 38          	mov    0x38(%esp),%eax
 804eb1a:	89 c0                	mov    %eax,%eax
 804eb1c:	cd 80                	int    $0x80
	errno = (int) p.sc_errno;
 804eb1e:	e8 dd d9 ff ff       	call   804c500 <__errno>
 804eb23:	8b 54 24 34          	mov    0x34(%esp),%edx
 804eb27:	89 10                	mov    %edx,(%eax)
	
	return p.return_val;		
 804eb29:	8b 44 24 30          	mov    0x30(%esp),%eax
}	
 804eb2d:	83 c4 4c             	add    $0x4c,%esp
 804eb30:	c3                   	ret    
 804eb31:	90                   	nop
 804eb32:	90                   	nop
 804eb33:	90                   	nop

0804eb34 <_realloc_r>:
#if __STD_C
Void_t* rEALLOc(RARG Void_t* oldmem, size_t bytes)
#else
Void_t* rEALLOc(RARG oldmem, bytes) RDECL Void_t* oldmem; size_t bytes;
#endif
{
 804eb34:	55                   	push   %ebp
 804eb35:	57                   	push   %edi
 804eb36:	56                   	push   %esi
 804eb37:	53                   	push   %ebx
 804eb38:	83 ec 3c             	sub    $0x3c,%esp
 804eb3b:	8b 74 24 50          	mov    0x50(%esp),%esi
 804eb3f:	8b 5c 24 54          	mov    0x54(%esp),%ebx
 804eb43:	8b 6c 24 58          	mov    0x58(%esp),%ebp
  if (bytes == 0) { fREe(RCALL oldmem); return 0; }
#endif


  /* realloc of null is supposed to be same as malloc */
  if (oldmem == 0) return mALLOc(RCALL bytes);
 804eb47:	85 db                	test   %ebx,%ebx
 804eb49:	0f 84 59 02 00 00    	je     804eda8 <_realloc_r+0x274>

  MALLOC_LOCK;
 804eb4f:	89 34 24             	mov    %esi,(%esp)
 804eb52:	e8 51 f2 ff ff       	call   804dda8 <__malloc_lock>

  newp    = oldp    = mem2chunk(oldmem);
 804eb57:	8d 4b f8             	lea    -0x8(%ebx),%ecx
 804eb5a:	89 4c 24 20          	mov    %ecx,0x20(%esp)
  newsize = oldsize = chunksize(oldp);
 804eb5e:	8b 53 fc             	mov    -0x4(%ebx),%edx
 804eb61:	89 d1                	mov    %edx,%ecx
 804eb63:	83 e1 fc             	and    $0xfffffffc,%ecx
 804eb66:	89 4c 24 1c          	mov    %ecx,0x1c(%esp)


  nb = request2size(bytes);
 804eb6a:	8d 45 0b             	lea    0xb(%ebp),%eax
 804eb6d:	83 f8 16             	cmp    $0x16,%eax
 804eb70:	0f 87 f2 00 00 00    	ja     804ec68 <_realloc_r+0x134>

  /* Check for overflow and just fail, if so. */
  if (nb > INT_MAX || nb < bytes)
 804eb76:	c7 44 24 24 10 00 00 	movl   $0x10,0x24(%esp)
 804eb7d:	00 

  newp    = oldp    = mem2chunk(oldmem);
  newsize = oldsize = chunksize(oldp);


  nb = request2size(bytes);
 804eb7e:	c7 44 24 18 10 00 00 	movl   $0x10,0x18(%esp)
 804eb85:	00 

  /* Check for overflow and just fail, if so. */
  if (nb > INT_MAX || nb < bytes)
 804eb86:	3b 6c 24 18          	cmp    0x18(%esp),%ebp
 804eb8a:	0f 87 eb 00 00 00    	ja     804ec7b <_realloc_r+0x147>
  }
#endif

  check_inuse_chunk(oldp);

  if ((long)(oldsize) < (long)(nb))  
 804eb90:	8b 44 24 24          	mov    0x24(%esp),%eax
 804eb94:	39 44 24 1c          	cmp    %eax,0x1c(%esp)
 804eb98:	0f 8d f2 00 00 00    	jge    804ec90 <_realloc_r+0x15c>
  {

    /* Try expanding forward */

    next = chunk_at_offset(oldp, oldsize);
 804eb9e:	8b 44 24 20          	mov    0x20(%esp),%eax
 804eba2:	03 44 24 1c          	add    0x1c(%esp),%eax
    if (next == top || !inuse(next)) 
 804eba6:	8b 0d 68 45 05 08    	mov    0x8054568,%ecx
 804ebac:	89 4c 24 28          	mov    %ecx,0x28(%esp)
 804ebb0:	39 c1                	cmp    %eax,%ecx
 804ebb2:	0f 84 00 02 00 00    	je     804edb8 <_realloc_r+0x284>
 804ebb8:	8b 48 04             	mov    0x4(%eax),%ecx
 804ebbb:	89 cf                	mov    %ecx,%edi
 804ebbd:	83 e7 fe             	and    $0xfffffffe,%edi
 804ebc0:	f6 44 38 04 01       	testb  $0x1,0x4(%eax,%edi,1)
 804ebc5:	0f 84 f9 00 00 00    	je     804ecc4 <_realloc_r+0x190>
      }
    }
    else
    {
      next = 0;
      nextsize = 0;
 804ebcb:	c7 44 24 2c 00 00 00 	movl   $0x0,0x2c(%esp)
 804ebd2:	00 
        goto split;
      }
    }
    else
    {
      next = 0;
 804ebd3:	31 c0                	xor    %eax,%eax
      nextsize = 0;
    }

    /* Try shifting backwards. */

    if (!prev_inuse(oldp))
 804ebd5:	83 e2 01             	and    $0x1,%edx
 804ebd8:	0f 85 54 01 00 00    	jne    804ed32 <_realloc_r+0x1fe>
    {
      prev = prev_chunk(oldp);
 804ebde:	8b 4c 24 20          	mov    0x20(%esp),%ecx
 804ebe2:	2b 4b f8             	sub    -0x8(%ebx),%ecx
      prevsize = chunksize(prev);
 804ebe5:	8b 51 04             	mov    0x4(%ecx),%edx
 804ebe8:	83 e2 fc             	and    $0xfffffffc,%edx
      if (next != 0)
      {
        /* into top */
        if (next == top)
        {
          if ((long)(nextsize + prevsize + newsize) >= (long)(nb + MINSIZE))
 804ebeb:	03 54 24 1c          	add    0x1c(%esp),%edx
      prev = prev_chunk(oldp);
      prevsize = chunksize(prev);

      /* try forward + backward first to save a later consolidation */

      if (next != 0)
 804ebef:	85 c0                	test   %eax,%eax
 804ebf1:	0f 84 29 01 00 00    	je     804ed20 <_realloc_r+0x1ec>
      {
        /* into top */
        if (next == top)
 804ebf7:	3b 44 24 28          	cmp    0x28(%esp),%eax
        {
          if ((long)(nextsize + prevsize + newsize) >= (long)(nb + MINSIZE))
 804ebfb:	89 54 24 28          	mov    %edx,0x28(%esp)
      /* try forward + backward first to save a later consolidation */

      if (next != 0)
      {
        /* into top */
        if (next == top)
 804ebff:	0f 84 c7 02 00 00    	je     804eecc <_realloc_r+0x398>
            return newmem;
          }
        }

        /* into next chunk */
        else if (((long)(nextsize + prevsize + newsize) >= (long)(nb)))
 804ec05:	8b 7c 24 2c          	mov    0x2c(%esp),%edi
 804ec09:	01 d7                	add    %edx,%edi
 804ec0b:	3b 7c 24 24          	cmp    0x24(%esp),%edi
 804ec0f:	0f 8c 0f 01 00 00    	jl     804ed24 <_realloc_r+0x1f0>
        {
          unlink(next, bck, fwd);
 804ec15:	8b 50 0c             	mov    0xc(%eax),%edx
 804ec18:	8b 40 08             	mov    0x8(%eax),%eax
 804ec1b:	89 50 0c             	mov    %edx,0xc(%eax)
 804ec1e:	89 42 08             	mov    %eax,0x8(%edx)
          unlink(prev, bck, fwd);
 804ec21:	8b 41 0c             	mov    0xc(%ecx),%eax
 804ec24:	8b 51 08             	mov    0x8(%ecx),%edx
 804ec27:	89 42 0c             	mov    %eax,0xc(%edx)
 804ec2a:	89 50 08             	mov    %edx,0x8(%eax)
          newp = prev;
          newsize += nextsize + prevsize;
          newmem = chunk2mem(newp);
 804ec2d:	8d 69 08             	lea    0x8(%ecx),%ebp
          MALLOC_COPY(newmem, oldmem, oldsize - SIZE_SZ);
 804ec30:	8b 44 24 1c          	mov    0x1c(%esp),%eax
 804ec34:	83 e8 04             	sub    $0x4,%eax
 804ec37:	83 f8 24             	cmp    $0x24,%eax
 804ec3a:	0f 87 64 03 00 00    	ja     804efa4 <_realloc_r+0x470>
 804ec40:	83 f8 13             	cmp    $0x13,%eax
 804ec43:	0f 86 1b 02 00 00    	jbe    804ee64 <_realloc_r+0x330>
 804ec49:	8b 13                	mov    (%ebx),%edx
 804ec4b:	89 51 08             	mov    %edx,0x8(%ecx)
 804ec4e:	8b 53 04             	mov    0x4(%ebx),%edx
 804ec51:	89 51 0c             	mov    %edx,0xc(%ecx)
 804ec54:	83 f8 1b             	cmp    $0x1b,%eax
 804ec57:	0f 87 98 03 00 00    	ja     804eff5 <_realloc_r+0x4c1>
 804ec5d:	8d 41 10             	lea    0x10(%ecx),%eax
 804ec60:	83 c3 08             	add    $0x8,%ebx
 804ec63:	e9 fe 01 00 00       	jmp    804ee66 <_realloc_r+0x332>

  newp    = oldp    = mem2chunk(oldmem);
  newsize = oldsize = chunksize(oldp);


  nb = request2size(bytes);
 804ec68:	83 e0 f8             	and    $0xfffffff8,%eax
 804ec6b:	89 44 24 18          	mov    %eax,0x18(%esp)

  /* Check for overflow and just fail, if so. */
  if (nb > INT_MAX || nb < bytes)
 804ec6f:	89 44 24 24          	mov    %eax,0x24(%esp)
 804ec73:	85 c0                	test   %eax,%eax
 804ec75:	0f 89 0b ff ff ff    	jns    804eb86 <_realloc_r+0x52>
  {
    RERRNO = ENOMEM;
 804ec7b:	c7 06 0c 00 00 00    	movl   $0xc,(%esi)
    return 0;
 804ec81:	31 ff                	xor    %edi,%edi
  check_inuse_chunk(newp);
  MALLOC_UNLOCK;
  return chunk2mem(newp);

#endif /* MALLOC_PROVIDED */
}
 804ec83:	89 f8                	mov    %edi,%eax
 804ec85:	83 c4 3c             	add    $0x3c,%esp
 804ec88:	5b                   	pop    %ebx
 804ec89:	5e                   	pop    %esi
 804ec8a:	5f                   	pop    %edi
 804ec8b:	5d                   	pop    %ebp
 804ec8c:	c3                   	ret    
 804ec8d:	8d 76 00             	lea    0x0(%esi),%esi
  if (oldmem == 0) return mALLOc(RCALL bytes);

  MALLOC_LOCK;

  newp    = oldp    = mem2chunk(oldmem);
  newsize = oldsize = chunksize(oldp);
 804ec90:	8b 7c 24 1c          	mov    0x1c(%esp),%edi
  /* realloc of null is supposed to be same as malloc */
  if (oldmem == 0) return mALLOc(RCALL bytes);

  MALLOC_LOCK;

  newp    = oldp    = mem2chunk(oldmem);
 804ec94:	8b 4c 24 20          	mov    0x20(%esp),%ecx
  }


 split:  /* split off extra room in old or expanded chunk */

  remainder_size = long_sub_size_t(newsize, nb);
 804ec98:	89 f8                	mov    %edi,%eax
 804ec9a:	2b 44 24 18          	sub    0x18(%esp),%eax

  if (remainder_size >= (long)MINSIZE) /* split off remainder */
 804ec9e:	83 f8 0f             	cmp    $0xf,%eax
 804eca1:	77 4d                	ja     804ecf0 <_realloc_r+0x1bc>
    set_inuse_bit_at_offset(remainder, remainder_size);
    fREe(RCALL chunk2mem(remainder)); /* let free() deal with it */
  }
  else
  {
    set_head_size(newp, newsize);
 804eca3:	83 e2 01             	and    $0x1,%edx
 804eca6:	09 fa                	or     %edi,%edx
 804eca8:	89 51 04             	mov    %edx,0x4(%ecx)
    set_inuse_bit_at_offset(newp, newsize);
 804ecab:	83 4c 39 04 01       	orl    $0x1,0x4(%ecx,%edi,1)
  }

  check_inuse_chunk(newp);
  MALLOC_UNLOCK;
 804ecb0:	89 34 24             	mov    %esi,(%esp)
 804ecb3:	e8 f4 f0 ff ff       	call   804ddac <__malloc_unlock>
  return chunk2mem(newp);
 804ecb8:	89 df                	mov    %ebx,%edi

#endif /* MALLOC_PROVIDED */
}
 804ecba:	89 f8                	mov    %edi,%eax
 804ecbc:	83 c4 3c             	add    $0x3c,%esp
 804ecbf:	5b                   	pop    %ebx
 804ecc0:	5e                   	pop    %esi
 804ecc1:	5f                   	pop    %edi
 804ecc2:	5d                   	pop    %ebp
 804ecc3:	c3                   	ret    
    /* Try expanding forward */

    next = chunk_at_offset(oldp, oldsize);
    if (next == top || !inuse(next)) 
    {
      nextsize = chunksize(next);
 804ecc4:	83 e1 fc             	and    $0xfffffffc,%ecx
 804ecc7:	89 4c 24 2c          	mov    %ecx,0x2c(%esp)
          return chunk2mem(oldp);
        }
      }

      /* Forward into next chunk */
      else if (((long)(nextsize + newsize) >= (long)(nb)))
 804eccb:	8b 7c 24 1c          	mov    0x1c(%esp),%edi
 804eccf:	01 cf                	add    %ecx,%edi
 804ecd1:	3b 7c 24 24          	cmp    0x24(%esp),%edi
 804ecd5:	0f 8c fa fe ff ff    	jl     804ebd5 <_realloc_r+0xa1>
      { 
        unlink(next, bck, fwd);
 804ecdb:	8b 48 0c             	mov    0xc(%eax),%ecx
 804ecde:	8b 40 08             	mov    0x8(%eax),%eax
 804ece1:	89 48 0c             	mov    %ecx,0xc(%eax)
 804ece4:	89 41 08             	mov    %eax,0x8(%ecx)
  /* realloc of null is supposed to be same as malloc */
  if (oldmem == 0) return mALLOc(RCALL bytes);

  MALLOC_LOCK;

  newp    = oldp    = mem2chunk(oldmem);
 804ece7:	8b 4c 24 20          	mov    0x20(%esp),%ecx
      /* Forward into next chunk */
      else if (((long)(nextsize + newsize) >= (long)(nb)))
      { 
        unlink(next, bck, fwd);
        newsize  += nextsize;
        goto split;
 804eceb:	eb ab                	jmp    804ec98 <_realloc_r+0x164>
 804eced:	8d 76 00             	lea    0x0(%esi),%esi

  remainder_size = long_sub_size_t(newsize, nb);

  if (remainder_size >= (long)MINSIZE) /* split off remainder */
  {
    remainder = chunk_at_offset(newp, nb);
 804ecf0:	8b 7c 24 18          	mov    0x18(%esp),%edi
 804ecf4:	01 cf                	add    %ecx,%edi
    set_head_size(newp, nb);
 804ecf6:	83 e2 01             	and    $0x1,%edx
 804ecf9:	0b 54 24 18          	or     0x18(%esp),%edx
 804ecfd:	89 51 04             	mov    %edx,0x4(%ecx)
    set_head(remainder, remainder_size | PREV_INUSE);
 804ed00:	89 c2                	mov    %eax,%edx
 804ed02:	83 ca 01             	or     $0x1,%edx
 804ed05:	89 57 04             	mov    %edx,0x4(%edi)
    set_inuse_bit_at_offset(remainder, remainder_size);
 804ed08:	83 4c 07 04 01       	orl    $0x1,0x4(%edi,%eax,1)
    fREe(RCALL chunk2mem(remainder)); /* let free() deal with it */
 804ed0d:	83 c7 08             	add    $0x8,%edi
 804ed10:	89 7c 24 04          	mov    %edi,0x4(%esp)
 804ed14:	89 34 24             	mov    %esi,(%esp)
 804ed17:	e8 e4 dd ff ff       	call   804cb00 <_free_r>
 804ed1c:	eb 92                	jmp    804ecb0 <_realloc_r+0x17c>
 804ed1e:	66 90                	xchg   %ax,%ax
 804ed20:	89 54 24 28          	mov    %edx,0x28(%esp)
          goto split;
        }
      }
      
      /* backward only */
      if (prev != 0 && (long)(prevsize + newsize) >= (long)nb)  
 804ed24:	8b 44 24 24          	mov    0x24(%esp),%eax
 804ed28:	39 44 24 28          	cmp    %eax,0x28(%esp)
 804ed2c:	0f 8d b2 00 00 00    	jge    804ede4 <_realloc_r+0x2b0>
      }
    }

    /* Must allocate */

    newmem = mALLOc (RCALL bytes);
 804ed32:	89 6c 24 04          	mov    %ebp,0x4(%esp)
 804ed36:	89 34 24             	mov    %esi,(%esp)
 804ed39:	e8 7e e7 ff ff       	call   804d4bc <_malloc_r>
 804ed3e:	89 c7                	mov    %eax,%edi

    if (newmem == 0)  /* propagate failure */
 804ed40:	85 c0                	test   %eax,%eax
 804ed42:	74 50                	je     804ed94 <_realloc_r+0x260>
    }

    /* Avoid copy if newp is next chunk after oldp. */
    /* (This can only happen when new chunk is sbrk'ed.) */

    if ( (newp = mem2chunk(newmem)) == next_chunk(oldp)) 
 804ed44:	8b 53 fc             	mov    -0x4(%ebx),%edx
 804ed47:	8d 48 f8             	lea    -0x8(%eax),%ecx
 804ed4a:	89 d0                	mov    %edx,%eax
 804ed4c:	83 e0 fe             	and    $0xfffffffe,%eax
 804ed4f:	03 44 24 20          	add    0x20(%esp),%eax
 804ed53:	39 c1                	cmp    %eax,%ecx
 804ed55:	0f 84 35 02 00 00    	je     804ef90 <_realloc_r+0x45c>
      newp = oldp;
      goto split;
    }

    /* Otherwise copy, free, and exit */
    MALLOC_COPY(newmem, oldmem, oldsize - SIZE_SZ);
 804ed5b:	8b 44 24 1c          	mov    0x1c(%esp),%eax
 804ed5f:	83 e8 04             	sub    $0x4,%eax
 804ed62:	83 f8 24             	cmp    $0x24,%eax
 804ed65:	0f 87 49 01 00 00    	ja     804eeb4 <_realloc_r+0x380>
 804ed6b:	83 f8 13             	cmp    $0x13,%eax
 804ed6e:	0f 87 d0 00 00 00    	ja     804ee44 <_realloc_r+0x310>
 804ed74:	89 f8                	mov    %edi,%eax
 804ed76:	89 da                	mov    %ebx,%edx
 804ed78:	8b 0a                	mov    (%edx),%ecx
 804ed7a:	89 08                	mov    %ecx,(%eax)
 804ed7c:	8b 4a 04             	mov    0x4(%edx),%ecx
 804ed7f:	89 48 04             	mov    %ecx,0x4(%eax)
 804ed82:	8b 52 08             	mov    0x8(%edx),%edx
 804ed85:	89 50 08             	mov    %edx,0x8(%eax)
    fREe(RCALL oldmem);
 804ed88:	89 5c 24 04          	mov    %ebx,0x4(%esp)
 804ed8c:	89 34 24             	mov    %esi,(%esp)
 804ed8f:	e8 6c dd ff ff       	call   804cb00 <_free_r>
    MALLOC_UNLOCK;
 804ed94:	89 34 24             	mov    %esi,(%esp)
 804ed97:	e8 10 f0 ff ff       	call   804ddac <__malloc_unlock>
  check_inuse_chunk(newp);
  MALLOC_UNLOCK;
  return chunk2mem(newp);

#endif /* MALLOC_PROVIDED */
}
 804ed9c:	89 f8                	mov    %edi,%eax
 804ed9e:	83 c4 3c             	add    $0x3c,%esp
 804eda1:	5b                   	pop    %ebx
 804eda2:	5e                   	pop    %esi
 804eda3:	5f                   	pop    %edi
 804eda4:	5d                   	pop    %ebp
 804eda5:	c3                   	ret    
 804eda6:	66 90                	xchg   %ax,%ax
  if (bytes == 0) { fREe(RCALL oldmem); return 0; }
#endif


  /* realloc of null is supposed to be same as malloc */
  if (oldmem == 0) return mALLOc(RCALL bytes);
 804eda8:	89 6c 24 54          	mov    %ebp,0x54(%esp)
  check_inuse_chunk(newp);
  MALLOC_UNLOCK;
  return chunk2mem(newp);

#endif /* MALLOC_PROVIDED */
}
 804edac:	83 c4 3c             	add    $0x3c,%esp
 804edaf:	5b                   	pop    %ebx
 804edb0:	5e                   	pop    %esi
 804edb1:	5f                   	pop    %edi
 804edb2:	5d                   	pop    %ebp
  if (bytes == 0) { fREe(RCALL oldmem); return 0; }
#endif


  /* realloc of null is supposed to be same as malloc */
  if (oldmem == 0) return mALLOc(RCALL bytes);
 804edb3:	e9 04 e7 ff ff       	jmp    804d4bc <_malloc_r>
    /* Try expanding forward */

    next = chunk_at_offset(oldp, oldsize);
    if (next == top || !inuse(next)) 
    {
      nextsize = chunksize(next);
 804edb8:	8b 4c 24 28          	mov    0x28(%esp),%ecx
 804edbc:	8b 49 04             	mov    0x4(%ecx),%ecx
 804edbf:	83 e1 fc             	and    $0xfffffffc,%ecx
 804edc2:	89 4c 24 2c          	mov    %ecx,0x2c(%esp)

      /* Forward into top only if a remainder */
      if (next == top)
      {
        if ((long)(nextsize + newsize) >= (long)(nb + MINSIZE))
 804edc6:	03 4c 24 1c          	add    0x1c(%esp),%ecx
 804edca:	8b 44 24 18          	mov    0x18(%esp),%eax
 804edce:	83 c0 10             	add    $0x10,%eax
 804edd1:	39 c1                	cmp    %eax,%ecx
 804edd3:	0f 8d a7 00 00 00    	jge    804ee80 <_realloc_r+0x34c>
 804edd9:	8b 44 24 28          	mov    0x28(%esp),%eax
 804eddd:	e9 f3 fd ff ff       	jmp    804ebd5 <_realloc_r+0xa1>
 804ede2:	66 90                	xchg   %ax,%ax
      }
      
      /* backward only */
      if (prev != 0 && (long)(prevsize + newsize) >= (long)nb)  
      {
        unlink(prev, bck, fwd);
 804ede4:	8b 41 0c             	mov    0xc(%ecx),%eax
 804ede7:	8b 51 08             	mov    0x8(%ecx),%edx
 804edea:	89 42 0c             	mov    %eax,0xc(%edx)
 804eded:	89 50 08             	mov    %edx,0x8(%eax)
        newp = prev;
        newsize += prevsize;
        newmem = chunk2mem(newp);
 804edf0:	8d 79 08             	lea    0x8(%ecx),%edi
        MALLOC_COPY(newmem, oldmem, oldsize - SIZE_SZ);
 804edf3:	8b 44 24 1c          	mov    0x1c(%esp),%eax
 804edf7:	83 e8 04             	sub    $0x4,%eax
 804edfa:	83 f8 24             	cmp    $0x24,%eax
 804edfd:	0f 87 cc 01 00 00    	ja     804efcf <_realloc_r+0x49b>
 804ee03:	83 f8 13             	cmp    $0x13,%eax
 804ee06:	0f 86 bc 01 00 00    	jbe    804efc8 <_realloc_r+0x494>
 804ee0c:	8b 13                	mov    (%ebx),%edx
 804ee0e:	89 51 08             	mov    %edx,0x8(%ecx)
 804ee11:	8b 53 04             	mov    0x4(%ebx),%edx
 804ee14:	89 51 0c             	mov    %edx,0xc(%ecx)
 804ee17:	83 f8 1b             	cmp    $0x1b,%eax
 804ee1a:	0f 87 08 02 00 00    	ja     804f028 <_realloc_r+0x4f4>
 804ee20:	8d 41 10             	lea    0x10(%ecx),%eax
 804ee23:	83 c3 08             	add    $0x8,%ebx
 804ee26:	8b 13                	mov    (%ebx),%edx
 804ee28:	89 10                	mov    %edx,(%eax)
 804ee2a:	8b 53 04             	mov    0x4(%ebx),%edx
 804ee2d:	89 50 04             	mov    %edx,0x4(%eax)
 804ee30:	8b 53 08             	mov    0x8(%ebx),%edx
 804ee33:	89 50 08             	mov    %edx,0x8(%eax)
 804ee36:	8b 51 04             	mov    0x4(%ecx),%edx
 804ee39:	89 fb                	mov    %edi,%ebx
      /* backward only */
      if (prev != 0 && (long)(prevsize + newsize) >= (long)nb)  
      {
        unlink(prev, bck, fwd);
        newp = prev;
        newsize += prevsize;
 804ee3b:	8b 7c 24 28          	mov    0x28(%esp),%edi
 804ee3f:	e9 54 fe ff ff       	jmp    804ec98 <_realloc_r+0x164>
      newp = oldp;
      goto split;
    }

    /* Otherwise copy, free, and exit */
    MALLOC_COPY(newmem, oldmem, oldsize - SIZE_SZ);
 804ee44:	8b 13                	mov    (%ebx),%edx
 804ee46:	89 17                	mov    %edx,(%edi)
 804ee48:	8b 53 04             	mov    0x4(%ebx),%edx
 804ee4b:	89 57 04             	mov    %edx,0x4(%edi)
 804ee4e:	83 f8 1b             	cmp    $0x1b,%eax
 804ee51:	0f 87 19 01 00 00    	ja     804ef70 <_realloc_r+0x43c>
 804ee57:	8d 47 08             	lea    0x8(%edi),%eax
 804ee5a:	8d 53 08             	lea    0x8(%ebx),%edx
 804ee5d:	e9 16 ff ff ff       	jmp    804ed78 <_realloc_r+0x244>
 804ee62:	66 90                	xchg   %ax,%ax
          unlink(next, bck, fwd);
          unlink(prev, bck, fwd);
          newp = prev;
          newsize += nextsize + prevsize;
          newmem = chunk2mem(newp);
          MALLOC_COPY(newmem, oldmem, oldsize - SIZE_SZ);
 804ee64:	89 e8                	mov    %ebp,%eax
 804ee66:	8b 13                	mov    (%ebx),%edx
 804ee68:	89 10                	mov    %edx,(%eax)
 804ee6a:	8b 53 04             	mov    0x4(%ebx),%edx
 804ee6d:	89 50 04             	mov    %edx,0x4(%eax)
 804ee70:	8b 53 08             	mov    0x8(%ebx),%edx
 804ee73:	89 50 08             	mov    %edx,0x8(%eax)
 804ee76:	8b 51 04             	mov    0x4(%ecx),%edx
 804ee79:	89 eb                	mov    %ebp,%ebx
 804ee7b:	e9 18 fe ff ff       	jmp    804ec98 <_realloc_r+0x164>
      if (next == top)
      {
        if ((long)(nextsize + newsize) >= (long)(nb + MINSIZE))
        {
          newsize += nextsize;
          top = chunk_at_offset(oldp, nb);
 804ee80:	8b 44 24 20          	mov    0x20(%esp),%eax
 804ee84:	03 44 24 18          	add    0x18(%esp),%eax
 804ee88:	a3 68 45 05 08       	mov    %eax,0x8054568
          set_head(top, (newsize - nb) | PREV_INUSE);
 804ee8d:	2b 4c 24 18          	sub    0x18(%esp),%ecx
 804ee91:	83 c9 01             	or     $0x1,%ecx
 804ee94:	89 48 04             	mov    %ecx,0x4(%eax)
          set_head_size(oldp, nb);
 804ee97:	8b 43 fc             	mov    -0x4(%ebx),%eax
 804ee9a:	83 e0 01             	and    $0x1,%eax
 804ee9d:	0b 44 24 18          	or     0x18(%esp),%eax
 804eea1:	89 43 fc             	mov    %eax,-0x4(%ebx)
	  MALLOC_UNLOCK;
 804eea4:	89 34 24             	mov    %esi,(%esp)
 804eea7:	e8 00 ef ff ff       	call   804ddac <__malloc_unlock>
          return chunk2mem(oldp);
 804eeac:	89 df                	mov    %ebx,%edi
 804eeae:	e9 07 fe ff ff       	jmp    804ecba <_realloc_r+0x186>
 804eeb3:	90                   	nop
      newp = oldp;
      goto split;
    }

    /* Otherwise copy, free, and exit */
    MALLOC_COPY(newmem, oldmem, oldsize - SIZE_SZ);
 804eeb4:	89 44 24 08          	mov    %eax,0x8(%esp)
 804eeb8:	89 5c 24 04          	mov    %ebx,0x4(%esp)
 804eebc:	89 3c 24             	mov    %edi,(%esp)
 804eebf:	e8 cc ed ff ff       	call   804dc90 <memmove>
 804eec4:	e9 bf fe ff ff       	jmp    804ed88 <_realloc_r+0x254>
 804eec9:	8d 76 00             	lea    0x0(%esi),%esi
      if (next != 0)
      {
        /* into top */
        if (next == top)
        {
          if ((long)(nextsize + prevsize + newsize) >= (long)(nb + MINSIZE))
 804eecc:	8b 44 24 2c          	mov    0x2c(%esp),%eax
 804eed0:	01 d0                	add    %edx,%eax
 804eed2:	89 44 24 2c          	mov    %eax,0x2c(%esp)
 804eed6:	8b 44 24 18          	mov    0x18(%esp),%eax
 804eeda:	83 c0 10             	add    $0x10,%eax
 804eedd:	39 44 24 2c          	cmp    %eax,0x2c(%esp)
 804eee1:	0f 8c 3d fe ff ff    	jl     804ed24 <_realloc_r+0x1f0>
          {
            unlink(prev, bck, fwd);
 804eee7:	8b 41 0c             	mov    0xc(%ecx),%eax
 804eeea:	8b 51 08             	mov    0x8(%ecx),%edx
 804eeed:	89 42 0c             	mov    %eax,0xc(%edx)
 804eef0:	89 50 08             	mov    %edx,0x8(%eax)
            newp = prev;
            newsize += prevsize + nextsize;
            newmem = chunk2mem(newp);
 804eef3:	8d 79 08             	lea    0x8(%ecx),%edi
            MALLOC_COPY(newmem, oldmem, oldsize - SIZE_SZ);
 804eef6:	8b 44 24 1c          	mov    0x1c(%esp),%eax
 804eefa:	83 e8 04             	sub    $0x4,%eax
 804eefd:	83 f8 24             	cmp    $0x24,%eax
 804ef00:	0f 87 73 01 00 00    	ja     804f079 <_realloc_r+0x545>
 804ef06:	83 f8 13             	cmp    $0x13,%eax
 804ef09:	0f 86 4c 01 00 00    	jbe    804f05b <_realloc_r+0x527>
 804ef0f:	8b 13                	mov    (%ebx),%edx
 804ef11:	89 51 08             	mov    %edx,0x8(%ecx)
 804ef14:	8b 53 04             	mov    0x4(%ebx),%edx
 804ef17:	89 51 0c             	mov    %edx,0xc(%ecx)
 804ef1a:	83 f8 1b             	cmp    $0x1b,%eax
 804ef1d:	0f 87 73 01 00 00    	ja     804f096 <_realloc_r+0x562>
 804ef23:	8d 41 10             	lea    0x10(%ecx),%eax
 804ef26:	83 c3 08             	add    $0x8,%ebx
 804ef29:	8b 13                	mov    (%ebx),%edx
 804ef2b:	89 10                	mov    %edx,(%eax)
 804ef2d:	8b 53 04             	mov    0x4(%ebx),%edx
 804ef30:	89 50 04             	mov    %edx,0x4(%eax)
 804ef33:	8b 53 08             	mov    0x8(%ebx),%edx
 804ef36:	89 50 08             	mov    %edx,0x8(%eax)
            top = chunk_at_offset(newp, nb);
 804ef39:	8b 54 24 18          	mov    0x18(%esp),%edx
 804ef3d:	01 ca                	add    %ecx,%edx
 804ef3f:	89 15 68 45 05 08    	mov    %edx,0x8054568
            set_head(top, (newsize - nb) | PREV_INUSE);
 804ef45:	8b 44 24 2c          	mov    0x2c(%esp),%eax
 804ef49:	2b 44 24 18          	sub    0x18(%esp),%eax
 804ef4d:	83 c8 01             	or     $0x1,%eax
 804ef50:	89 42 04             	mov    %eax,0x4(%edx)
            set_head_size(newp, nb);
 804ef53:	8b 41 04             	mov    0x4(%ecx),%eax
 804ef56:	83 e0 01             	and    $0x1,%eax
 804ef59:	0b 44 24 18          	or     0x18(%esp),%eax
 804ef5d:	89 41 04             	mov    %eax,0x4(%ecx)
	    MALLOC_UNLOCK;
 804ef60:	89 34 24             	mov    %esi,(%esp)
 804ef63:	e8 44 ee ff ff       	call   804ddac <__malloc_unlock>
            return newmem;
 804ef68:	e9 4d fd ff ff       	jmp    804ecba <_realloc_r+0x186>
 804ef6d:	8d 76 00             	lea    0x0(%esi),%esi
      newp = oldp;
      goto split;
    }

    /* Otherwise copy, free, and exit */
    MALLOC_COPY(newmem, oldmem, oldsize - SIZE_SZ);
 804ef70:	8b 53 08             	mov    0x8(%ebx),%edx
 804ef73:	89 57 08             	mov    %edx,0x8(%edi)
 804ef76:	8b 53 0c             	mov    0xc(%ebx),%edx
 804ef79:	89 57 0c             	mov    %edx,0xc(%edi)
 804ef7c:	83 f8 24             	cmp    $0x24,%eax
 804ef7f:	0f 84 8c 00 00 00    	je     804f011 <_realloc_r+0x4dd>
 804ef85:	8d 47 10             	lea    0x10(%edi),%eax
 804ef88:	8d 53 10             	lea    0x10(%ebx),%edx
 804ef8b:	e9 e8 fd ff ff       	jmp    804ed78 <_realloc_r+0x244>
    /* Avoid copy if newp is next chunk after oldp. */
    /* (This can only happen when new chunk is sbrk'ed.) */

    if ( (newp = mem2chunk(newmem)) == next_chunk(oldp)) 
    {
      newsize += chunksize(newp);
 804ef90:	8b 7f fc             	mov    -0x4(%edi),%edi
 804ef93:	83 e7 fc             	and    $0xfffffffc,%edi
 804ef96:	03 7c 24 1c          	add    0x1c(%esp),%edi
      newp = oldp;
 804ef9a:	8b 4c 24 20          	mov    0x20(%esp),%ecx
      goto split;
 804ef9e:	e9 f5 fc ff ff       	jmp    804ec98 <_realloc_r+0x164>
 804efa3:	90                   	nop
          unlink(next, bck, fwd);
          unlink(prev, bck, fwd);
          newp = prev;
          newsize += nextsize + prevsize;
          newmem = chunk2mem(newp);
          MALLOC_COPY(newmem, oldmem, oldsize - SIZE_SZ);
 804efa4:	89 44 24 08          	mov    %eax,0x8(%esp)
 804efa8:	89 5c 24 04          	mov    %ebx,0x4(%esp)
 804efac:	89 2c 24             	mov    %ebp,(%esp)
 804efaf:	89 4c 24 14          	mov    %ecx,0x14(%esp)
 804efb3:	e8 d8 ec ff ff       	call   804dc90 <memmove>
 804efb8:	8b 4c 24 14          	mov    0x14(%esp),%ecx
 804efbc:	8b 51 04             	mov    0x4(%ecx),%edx
 804efbf:	89 eb                	mov    %ebp,%ebx
 804efc1:	e9 d2 fc ff ff       	jmp    804ec98 <_realloc_r+0x164>
 804efc6:	66 90                	xchg   %ax,%ax
      {
        unlink(prev, bck, fwd);
        newp = prev;
        newsize += prevsize;
        newmem = chunk2mem(newp);
        MALLOC_COPY(newmem, oldmem, oldsize - SIZE_SZ);
 804efc8:	89 f8                	mov    %edi,%eax
 804efca:	e9 57 fe ff ff       	jmp    804ee26 <_realloc_r+0x2f2>
 804efcf:	89 44 24 08          	mov    %eax,0x8(%esp)
 804efd3:	89 5c 24 04          	mov    %ebx,0x4(%esp)
 804efd7:	89 3c 24             	mov    %edi,(%esp)
 804efda:	89 4c 24 14          	mov    %ecx,0x14(%esp)
 804efde:	e8 ad ec ff ff       	call   804dc90 <memmove>
 804efe3:	8b 4c 24 14          	mov    0x14(%esp),%ecx
 804efe7:	8b 51 04             	mov    0x4(%ecx),%edx
 804efea:	89 fb                	mov    %edi,%ebx
      /* backward only */
      if (prev != 0 && (long)(prevsize + newsize) >= (long)nb)  
      {
        unlink(prev, bck, fwd);
        newp = prev;
        newsize += prevsize;
 804efec:	8b 7c 24 28          	mov    0x28(%esp),%edi
 804eff0:	e9 a3 fc ff ff       	jmp    804ec98 <_realloc_r+0x164>
          unlink(next, bck, fwd);
          unlink(prev, bck, fwd);
          newp = prev;
          newsize += nextsize + prevsize;
          newmem = chunk2mem(newp);
          MALLOC_COPY(newmem, oldmem, oldsize - SIZE_SZ);
 804eff5:	8b 53 08             	mov    0x8(%ebx),%edx
 804eff8:	89 51 10             	mov    %edx,0x10(%ecx)
 804effb:	8b 53 0c             	mov    0xc(%ebx),%edx
 804effe:	89 51 14             	mov    %edx,0x14(%ecx)
 804f001:	83 f8 24             	cmp    $0x24,%eax
 804f004:	74 3e                	je     804f044 <_realloc_r+0x510>
 804f006:	8d 41 18             	lea    0x18(%ecx),%eax
 804f009:	83 c3 10             	add    $0x10,%ebx
 804f00c:	e9 55 fe ff ff       	jmp    804ee66 <_realloc_r+0x332>
      newp = oldp;
      goto split;
    }

    /* Otherwise copy, free, and exit */
    MALLOC_COPY(newmem, oldmem, oldsize - SIZE_SZ);
 804f011:	8b 43 10             	mov    0x10(%ebx),%eax
 804f014:	89 47 10             	mov    %eax,0x10(%edi)
 804f017:	8b 43 14             	mov    0x14(%ebx),%eax
 804f01a:	89 47 14             	mov    %eax,0x14(%edi)
 804f01d:	8d 47 18             	lea    0x18(%edi),%eax
 804f020:	8d 53 18             	lea    0x18(%ebx),%edx
 804f023:	e9 50 fd ff ff       	jmp    804ed78 <_realloc_r+0x244>
      {
        unlink(prev, bck, fwd);
        newp = prev;
        newsize += prevsize;
        newmem = chunk2mem(newp);
        MALLOC_COPY(newmem, oldmem, oldsize - SIZE_SZ);
 804f028:	8b 53 08             	mov    0x8(%ebx),%edx
 804f02b:	89 51 10             	mov    %edx,0x10(%ecx)
 804f02e:	8b 53 0c             	mov    0xc(%ebx),%edx
 804f031:	89 51 14             	mov    %edx,0x14(%ecx)
 804f034:	83 f8 24             	cmp    $0x24,%eax
 804f037:	74 29                	je     804f062 <_realloc_r+0x52e>
 804f039:	8d 41 18             	lea    0x18(%ecx),%eax
 804f03c:	83 c3 10             	add    $0x10,%ebx
 804f03f:	e9 e2 fd ff ff       	jmp    804ee26 <_realloc_r+0x2f2>
          unlink(next, bck, fwd);
          unlink(prev, bck, fwd);
          newp = prev;
          newsize += nextsize + prevsize;
          newmem = chunk2mem(newp);
          MALLOC_COPY(newmem, oldmem, oldsize - SIZE_SZ);
 804f044:	8b 43 10             	mov    0x10(%ebx),%eax
 804f047:	89 41 18             	mov    %eax,0x18(%ecx)
 804f04a:	8b 43 14             	mov    0x14(%ebx),%eax
 804f04d:	89 41 1c             	mov    %eax,0x1c(%ecx)
 804f050:	8d 41 20             	lea    0x20(%ecx),%eax
 804f053:	83 c3 18             	add    $0x18,%ebx
 804f056:	e9 0b fe ff ff       	jmp    804ee66 <_realloc_r+0x332>
          {
            unlink(prev, bck, fwd);
            newp = prev;
            newsize += prevsize + nextsize;
            newmem = chunk2mem(newp);
            MALLOC_COPY(newmem, oldmem, oldsize - SIZE_SZ);
 804f05b:	89 f8                	mov    %edi,%eax
 804f05d:	e9 c7 fe ff ff       	jmp    804ef29 <_realloc_r+0x3f5>
      {
        unlink(prev, bck, fwd);
        newp = prev;
        newsize += prevsize;
        newmem = chunk2mem(newp);
        MALLOC_COPY(newmem, oldmem, oldsize - SIZE_SZ);
 804f062:	8b 43 10             	mov    0x10(%ebx),%eax
 804f065:	89 41 18             	mov    %eax,0x18(%ecx)
 804f068:	8b 43 14             	mov    0x14(%ebx),%eax
 804f06b:	89 41 1c             	mov    %eax,0x1c(%ecx)
 804f06e:	8d 41 20             	lea    0x20(%ecx),%eax
 804f071:	83 c3 18             	add    $0x18,%ebx
 804f074:	e9 ad fd ff ff       	jmp    804ee26 <_realloc_r+0x2f2>
          {
            unlink(prev, bck, fwd);
            newp = prev;
            newsize += prevsize + nextsize;
            newmem = chunk2mem(newp);
            MALLOC_COPY(newmem, oldmem, oldsize - SIZE_SZ);
 804f079:	89 44 24 08          	mov    %eax,0x8(%esp)
 804f07d:	89 5c 24 04          	mov    %ebx,0x4(%esp)
 804f081:	89 3c 24             	mov    %edi,(%esp)
 804f084:	89 4c 24 14          	mov    %ecx,0x14(%esp)
 804f088:	e8 03 ec ff ff       	call   804dc90 <memmove>
 804f08d:	8b 4c 24 14          	mov    0x14(%esp),%ecx
 804f091:	e9 a3 fe ff ff       	jmp    804ef39 <_realloc_r+0x405>
 804f096:	8b 53 08             	mov    0x8(%ebx),%edx
 804f099:	89 51 10             	mov    %edx,0x10(%ecx)
 804f09c:	8b 53 0c             	mov    0xc(%ebx),%edx
 804f09f:	89 51 14             	mov    %edx,0x14(%ecx)
 804f0a2:	83 f8 24             	cmp    $0x24,%eax
 804f0a5:	74 0b                	je     804f0b2 <_realloc_r+0x57e>
 804f0a7:	8d 41 18             	lea    0x18(%ecx),%eax
 804f0aa:	83 c3 10             	add    $0x10,%ebx
 804f0ad:	e9 77 fe ff ff       	jmp    804ef29 <_realloc_r+0x3f5>
 804f0b2:	8b 43 10             	mov    0x10(%ebx),%eax
 804f0b5:	89 41 18             	mov    %eax,0x18(%ecx)
 804f0b8:	8b 43 14             	mov    0x14(%ebx),%eax
 804f0bb:	89 41 1c             	mov    %eax,0x1c(%ecx)
 804f0be:	8d 41 20             	lea    0x20(%ecx),%eax
 804f0c1:	83 c3 18             	add    $0x18,%ebx
 804f0c4:	e9 60 fe ff ff       	jmp    804ef29 <_realloc_r+0x3f5>
 804f0c9:	90                   	nop
 804f0ca:	90                   	nop
 804f0cb:	90                   	nop

0804f0cc <_sbrk_r>:

void *
_DEFUN (_sbrk_r, (ptr, incr),
     struct _reent *ptr _AND
     ptrdiff_t incr)
{
 804f0cc:	83 ec 1c             	sub    $0x1c,%esp
  char *ret;
  void *_sbrk(ptrdiff_t);

  errno = 0;
 804f0cf:	c7 05 f4 49 05 08 00 	movl   $0x0,0x80549f4
 804f0d6:	00 00 00 
  if ((ret = (char *)(_sbrk (incr))) == (void *) -1 && errno != 0)
 804f0d9:	8b 44 24 24          	mov    0x24(%esp),%eax
 804f0dd:	89 04 24             	mov    %eax,(%esp)
 804f0e0:	e8 eb 99 ff ff       	call   8048ad0 <sbrk>
 804f0e5:	83 f8 ff             	cmp    $0xffffffff,%eax
 804f0e8:	74 06                	je     804f0f0 <_sbrk_r+0x24>
    ptr->_errno = errno;
  return ret;
}
 804f0ea:	83 c4 1c             	add    $0x1c,%esp
 804f0ed:	c3                   	ret    
 804f0ee:	66 90                	xchg   %ax,%ax
{
  char *ret;
  void *_sbrk(ptrdiff_t);

  errno = 0;
  if ((ret = (char *)(_sbrk (incr))) == (void *) -1 && errno != 0)
 804f0f0:	8b 15 f4 49 05 08    	mov    0x80549f4,%edx
 804f0f6:	85 d2                	test   %edx,%edx
 804f0f8:	74 f0                	je     804f0ea <_sbrk_r+0x1e>
    ptr->_errno = errno;
 804f0fa:	8b 4c 24 20          	mov    0x20(%esp),%ecx
 804f0fe:	89 11                	mov    %edx,(%ecx)
  return ret;
}
 804f100:	83 c4 1c             	add    $0x1c,%esp
 804f103:	c3                   	ret    

0804f104 <__fpclassifyd>:
int
__fpclassifyd (double x)
{
  __uint32_t msw, lsw;

  EXTRACT_WORDS(msw,lsw,x);
 804f104:	8b 44 24 04          	mov    0x4(%esp),%eax
 804f108:	8b 54 24 08          	mov    0x8(%esp),%edx

  if ((msw == 0x00000000 && lsw == 0x00000000) ||
 804f10c:	85 d2                	test   %edx,%edx
 804f10e:	74 1c                	je     804f12c <__fpclassifyd+0x28>
 804f110:	81 fa 00 00 00 80    	cmp    $0x80000000,%edx
 804f116:	74 14                	je     804f12c <__fpclassifyd+0x28>
      (msw == 0x80000000 && lsw == 0x00000000))
    return FP_ZERO;
  else if ((msw >= 0x00100000 && msw <= 0x7fefffff) ||
 804f118:	8d 8a 00 00 f0 ff    	lea    -0x100000(%edx),%ecx
 804f11e:	81 f9 ff ff df 7f    	cmp    $0x7fdfffff,%ecx
 804f124:	77 12                	ja     804f138 <__fpclassifyd+0x34>
           (msw >= 0x80100000 && msw <= 0xffefffff))
    return FP_NORMAL;
 804f126:	b8 04 00 00 00       	mov    $0x4,%eax
 804f12b:	c3                   	ret    
{
  __uint32_t msw, lsw;

  EXTRACT_WORDS(msw,lsw,x);

  if ((msw == 0x00000000 && lsw == 0x00000000) ||
 804f12c:	85 c0                	test   %eax,%eax
 804f12e:	75 e8                	jne    804f118 <__fpclassifyd+0x14>
      (msw == 0x80000000 && lsw == 0x00000000))
    return FP_ZERO;
 804f130:	b8 02 00 00 00       	mov    $0x2,%eax
 804f135:	c3                   	ret    
 804f136:	66 90                	xchg   %ax,%ax
  else if ((msw >= 0x00100000 && msw <= 0x7fefffff) ||
           (msw >= 0x80100000 && msw <= 0xffefffff))
 804f138:	8d 8a 00 00 f0 7f    	lea    0x7ff00000(%edx),%ecx
  EXTRACT_WORDS(msw,lsw,x);

  if ((msw == 0x00000000 && lsw == 0x00000000) ||
      (msw == 0x80000000 && lsw == 0x00000000))
    return FP_ZERO;
  else if ((msw >= 0x00100000 && msw <= 0x7fefffff) ||
 804f13e:	81 f9 ff ff df 7f    	cmp    $0x7fdfffff,%ecx
 804f144:	76 e0                	jbe    804f126 <__fpclassifyd+0x22>
           (msw >= 0x80100000 && msw <= 0xffefffff))
    return FP_NORMAL;
  else if ((msw >= 0x00000000 && msw <= 0x000fffff) ||
 804f146:	81 fa ff ff 0f 00    	cmp    $0xfffff,%edx
 804f14c:	77 06                	ja     804f154 <__fpclassifyd+0x50>
           (msw >= 0x80000000 && msw <= 0x800fffff))
    /* zero is already handled above */
    return FP_SUBNORMAL;
 804f14e:	b8 03 00 00 00       	mov    $0x3,%eax
 804f153:	c3                   	ret    
    return FP_ZERO;
  else if ((msw >= 0x00100000 && msw <= 0x7fefffff) ||
           (msw >= 0x80100000 && msw <= 0xffefffff))
    return FP_NORMAL;
  else if ((msw >= 0x00000000 && msw <= 0x000fffff) ||
           (msw >= 0x80000000 && msw <= 0x800fffff))
 804f154:	8d 8a 00 00 00 80    	lea    -0x80000000(%edx),%ecx
      (msw == 0x80000000 && lsw == 0x00000000))
    return FP_ZERO;
  else if ((msw >= 0x00100000 && msw <= 0x7fefffff) ||
           (msw >= 0x80100000 && msw <= 0xffefffff))
    return FP_NORMAL;
  else if ((msw >= 0x00000000 && msw <= 0x000fffff) ||
 804f15a:	81 f9 ff ff 0f 00    	cmp    $0xfffff,%ecx
 804f160:	76 ec                	jbe    804f14e <__fpclassifyd+0x4a>
           (msw >= 0x80000000 && msw <= 0x800fffff))
    /* zero is already handled above */
    return FP_SUBNORMAL;
  else if ((msw == 0x7ff00000 && lsw == 0x00000000) ||
 804f162:	81 fa 00 00 f0 7f    	cmp    $0x7ff00000,%edx
 804f168:	74 0b                	je     804f175 <__fpclassifyd+0x71>
 804f16a:	81 fa 00 00 f0 ff    	cmp    $0xfff00000,%edx
 804f170:	74 03                	je     804f175 <__fpclassifyd+0x71>
           (msw == 0xfff00000 && lsw == 0x00000000))
    return FP_INFINITE;
  else
    return FP_NAN;
 804f172:	31 c0                	xor    %eax,%eax
}
 804f174:	c3                   	ret    
    return FP_NORMAL;
  else if ((msw >= 0x00000000 && msw <= 0x000fffff) ||
           (msw >= 0x80000000 && msw <= 0x800fffff))
    /* zero is already handled above */
    return FP_SUBNORMAL;
  else if ((msw == 0x7ff00000 && lsw == 0x00000000) ||
 804f175:	85 c0                	test   %eax,%eax

  EXTRACT_WORDS(msw,lsw,x);

  if ((msw == 0x00000000 && lsw == 0x00000000) ||
      (msw == 0x80000000 && lsw == 0x00000000))
    return FP_ZERO;
 804f177:	0f 94 c0             	sete   %al
 804f17a:	0f b6 c0             	movzbl %al,%eax
 804f17d:	c3                   	ret    
 804f17e:	90                   	nop
 804f17f:	90                   	nop

0804f180 <__sread>:
_DEFUN(__sread, (ptr, cookie, buf, n),
       struct _reent *ptr _AND
       void *cookie _AND
       char *buf _AND
       _READ_WRITE_BUFSIZE_TYPE n)
{
 804f180:	53                   	push   %ebx
 804f181:	83 ec 18             	sub    $0x18,%esp
 804f184:	8b 5c 24 24          	mov    0x24(%esp),%ebx
  int oldmode = 0;
  if (fp->_flags & __SCLE)
    oldmode = setmode (fp->_file, O_BINARY);
#endif

  ret = _read_r (ptr, fp->_file, buf, n);
 804f188:	8b 44 24 2c          	mov    0x2c(%esp),%eax
 804f18c:	89 44 24 0c          	mov    %eax,0xc(%esp)
 804f190:	8b 44 24 28          	mov    0x28(%esp),%eax
 804f194:	89 44 24 08          	mov    %eax,0x8(%esp)
 804f198:	0f bf 43 0e          	movswl 0xe(%ebx),%eax
 804f19c:	89 44 24 04          	mov    %eax,0x4(%esp)
 804f1a0:	8b 44 24 20          	mov    0x20(%esp),%eax
 804f1a4:	89 04 24             	mov    %eax,(%esp)
 804f1a7:	e8 68 15 00 00       	call   8050714 <_read_r>
    setmode (fp->_file, oldmode);
#endif

  /* If the read succeeded, update the current offset.  */

  if (ret >= 0)
 804f1ac:	85 c0                	test   %eax,%eax
 804f1ae:	78 08                	js     804f1b8 <__sread+0x38>
    fp->_offset += ret;
 804f1b0:	01 43 50             	add    %eax,0x50(%ebx)
  else
    fp->_flags &= ~__SOFF;	/* paranoia */
  return ret;
}
 804f1b3:	83 c4 18             	add    $0x18,%esp
 804f1b6:	5b                   	pop    %ebx
 804f1b7:	c3                   	ret    
  /* If the read succeeded, update the current offset.  */

  if (ret >= 0)
    fp->_offset += ret;
  else
    fp->_flags &= ~__SOFF;	/* paranoia */
 804f1b8:	66 81 63 0c ff ef    	andw   $0xefff,0xc(%ebx)
  return ret;
}
 804f1be:	83 c4 18             	add    $0x18,%esp
 804f1c1:	5b                   	pop    %ebx
 804f1c2:	c3                   	ret    
 804f1c3:	90                   	nop

0804f1c4 <__seofread>:
       _PTR cookie _AND
       char *buf   _AND
       _READ_WRITE_BUFSIZE_TYPE len)
{
  return 0;
}
 804f1c4:	31 c0                	xor    %eax,%eax
 804f1c6:	c3                   	ret    
 804f1c7:	90                   	nop

0804f1c8 <__swrite>:
_DEFUN(__swrite, (ptr, cookie, buf, n),
       struct _reent *ptr _AND
       void *cookie _AND
       char const *buf _AND
       _READ_WRITE_BUFSIZE_TYPE n)
{
 804f1c8:	55                   	push   %ebp
 804f1c9:	57                   	push   %edi
 804f1ca:	56                   	push   %esi
 804f1cb:	53                   	push   %ebx
 804f1cc:	83 ec 1c             	sub    $0x1c,%esp
 804f1cf:	8b 74 24 30          	mov    0x30(%esp),%esi
 804f1d3:	8b 5c 24 34          	mov    0x34(%esp),%ebx
 804f1d7:	8b 7c 24 38          	mov    0x38(%esp),%edi
 804f1db:	8b 6c 24 3c          	mov    0x3c(%esp),%ebp
  ssize_t w;
#ifdef __SCLE
  int oldmode=0;
#endif

  if (fp->_flags & __SAPP)
 804f1df:	8b 43 0c             	mov    0xc(%ebx),%eax
 804f1e2:	f6 c4 01             	test   $0x1,%ah
 804f1e5:	74 23                	je     804f20a <__swrite+0x42>
    _lseek_r (ptr, fp->_file, (_off_t) 0, SEEK_END);
 804f1e7:	c7 44 24 0c 02 00 00 	movl   $0x2,0xc(%esp)
 804f1ee:	00 
 804f1ef:	c7 44 24 08 00 00 00 	movl   $0x0,0x8(%esp)
 804f1f6:	00 
 804f1f7:	0f bf 43 0e          	movswl 0xe(%ebx),%eax
 804f1fb:	89 44 24 04          	mov    %eax,0x4(%esp)
 804f1ff:	89 34 24             	mov    %esi,(%esp)
 804f202:	e8 c5 14 00 00       	call   80506cc <_lseek_r>
 804f207:	8b 43 0c             	mov    0xc(%ebx),%eax
  fp->_flags &= ~__SOFF;	/* in case O_APPEND mode is set */
 804f20a:	80 e4 ef             	and    $0xef,%ah
 804f20d:	66 89 43 0c          	mov    %ax,0xc(%ebx)
#ifdef __SCLE
  if (fp->_flags & __SCLE)
    oldmode = setmode (fp->_file, O_BINARY);
#endif

  w = _write_r (ptr, fp->_file, buf, n);
 804f211:	89 6c 24 3c          	mov    %ebp,0x3c(%esp)
 804f215:	89 7c 24 38          	mov    %edi,0x38(%esp)
 804f219:	0f bf 43 0e          	movswl 0xe(%ebx),%eax
 804f21d:	89 44 24 34          	mov    %eax,0x34(%esp)
 804f221:	89 74 24 30          	mov    %esi,0x30(%esp)
  if (oldmode)
    setmode (fp->_file, oldmode);
#endif

  return w;
}
 804f225:	83 c4 1c             	add    $0x1c,%esp
 804f228:	5b                   	pop    %ebx
 804f229:	5e                   	pop    %esi
 804f22a:	5f                   	pop    %edi
 804f22b:	5d                   	pop    %ebp
#ifdef __SCLE
  if (fp->_flags & __SCLE)
    oldmode = setmode (fp->_file, O_BINARY);
#endif

  w = _write_r (ptr, fp->_file, buf, n);
 804f22c:	e9 8b 10 00 00       	jmp    80502bc <_write_r>
 804f231:	8d 76 00             	lea    0x0(%esi),%esi

0804f234 <__sseek>:
_DEFUN(__sseek, (ptr, cookie, offset, whence),
       struct _reent *ptr _AND
       void *cookie _AND
       _fpos_t offset _AND
       int whence)
{
 804f234:	53                   	push   %ebx
 804f235:	83 ec 18             	sub    $0x18,%esp
 804f238:	8b 5c 24 24          	mov    0x24(%esp),%ebx
  register FILE *fp = (FILE *) cookie;
  register _off_t ret;

  ret = _lseek_r (ptr, fp->_file, (_off_t) offset, whence);
 804f23c:	8b 44 24 2c          	mov    0x2c(%esp),%eax
 804f240:	89 44 24 0c          	mov    %eax,0xc(%esp)
 804f244:	8b 44 24 28          	mov    0x28(%esp),%eax
 804f248:	89 44 24 08          	mov    %eax,0x8(%esp)
 804f24c:	0f bf 43 0e          	movswl 0xe(%ebx),%eax
 804f250:	89 44 24 04          	mov    %eax,0x4(%esp)
 804f254:	8b 44 24 20          	mov    0x20(%esp),%eax
 804f258:	89 04 24             	mov    %eax,(%esp)
 804f25b:	e8 6c 14 00 00       	call   80506cc <_lseek_r>
  if (ret == -1L)
 804f260:	83 f8 ff             	cmp    $0xffffffff,%eax
 804f263:	74 0f                	je     804f274 <__sseek+0x40>
    fp->_flags &= ~__SOFF;
  else
    {
      fp->_flags |= __SOFF;
 804f265:	66 81 4b 0c 00 10    	orw    $0x1000,0xc(%ebx)
      fp->_offset = ret;
 804f26b:	89 43 50             	mov    %eax,0x50(%ebx)
    }
  return ret;
}
 804f26e:	83 c4 18             	add    $0x18,%esp
 804f271:	5b                   	pop    %ebx
 804f272:	c3                   	ret    
 804f273:	90                   	nop
  register FILE *fp = (FILE *) cookie;
  register _off_t ret;

  ret = _lseek_r (ptr, fp->_file, (_off_t) offset, whence);
  if (ret == -1L)
    fp->_flags &= ~__SOFF;
 804f274:	66 81 63 0c ff ef    	andw   $0xefff,0xc(%ebx)
    {
      fp->_flags |= __SOFF;
      fp->_offset = ret;
    }
  return ret;
}
 804f27a:	83 c4 18             	add    $0x18,%esp
 804f27d:	5b                   	pop    %ebx
 804f27e:	c3                   	ret    
 804f27f:	90                   	nop

0804f280 <__sclose>:
       struct _reent *ptr _AND
       void *cookie)
{
  FILE *fp = (FILE *) cookie;

  return _close_r (ptr, fp->_file);
 804f280:	8b 44 24 08          	mov    0x8(%esp),%eax
 804f284:	0f bf 40 0e          	movswl 0xe(%eax),%eax
 804f288:	89 44 24 08          	mov    %eax,0x8(%esp)
 804f28c:	e9 2b 11 00 00       	jmp    80503bc <_close_r>
 804f291:	90                   	nop
 804f292:	90                   	nop
 804f293:	90                   	nop

0804f294 <strcmp>:

int
_DEFUN (strcmp, (s1, s2),
	_CONST char *s1 _AND
	_CONST char *s2)
{ 
 804f294:	53                   	push   %ebx
 804f295:	8b 44 24 08          	mov    0x8(%esp),%eax
 804f299:	8b 54 24 0c          	mov    0xc(%esp),%edx
#else
  unsigned long *a1;
  unsigned long *a2;

  /* If s1 or s2 are unaligned, then compare bytes. */
  if (!UNALIGNED (s1, s2))
 804f29d:	89 d1                	mov    %edx,%ecx
 804f29f:	09 c1                	or     %eax,%ecx
 804f2a1:	83 e1 03             	and    $0x3,%ecx
 804f2a4:	75 10                	jne    804f2b6 <strcmp+0x22>
    {  
      /* If s1 and s2 are word-aligned, compare them a word at a time. */
      a1 = (unsigned long*)s1;
      a2 = (unsigned long*)s2;
      while (*a1 == *a2)
 804f2a6:	8b 08                	mov    (%eax),%ecx
 804f2a8:	3b 0a                	cmp    (%edx),%ecx
 804f2aa:	74 28                	je     804f2d4 <strcmp+0x40>
 804f2ac:	eb 08                	jmp    804f2b6 <strcmp+0x22>
 804f2ae:	66 90                	xchg   %ax,%ax
      /* A difference was detected in last few bytes of s1, so search bytewise */
      s1 = (char*)a1;
      s2 = (char*)a2;
    }

  while (*s1 != '\0' && *s1 == *s2)
 804f2b0:	3a 0a                	cmp    (%edx),%cl
 804f2b2:	75 08                	jne    804f2bc <strcmp+0x28>
    {
      s1++;
 804f2b4:	40                   	inc    %eax
      s2++;
 804f2b5:	42                   	inc    %edx
      /* A difference was detected in last few bytes of s1, so search bytewise */
      s1 = (char*)a1;
      s2 = (char*)a2;
    }

  while (*s1 != '\0' && *s1 == *s2)
 804f2b6:	8a 08                	mov    (%eax),%cl
 804f2b8:	84 c9                	test   %cl,%cl
 804f2ba:	75 f4                	jne    804f2b0 <strcmp+0x1c>
    {
      s1++;
      s2++;
    }
  return (*(unsigned char *) s1) - (*(unsigned char *) s2);
 804f2bc:	0f b6 00             	movzbl (%eax),%eax
 804f2bf:	0f b6 12             	movzbl (%edx),%edx
 804f2c2:	29 d0                	sub    %edx,%eax
#endif /* not PREFER_SIZE_OVER_SPEED */
}
 804f2c4:	5b                   	pop    %ebx
 804f2c5:	c3                   	ret    
 804f2c6:	66 90                	xchg   %ax,%ax
          /* To get here, *a1 == *a2, thus if we find a null in *a1,
	     then the strings must be equal, so return zero.  */
          if (DETECTNULL (*a1))
	    return 0;

          a1++;
 804f2c8:	83 c0 04             	add    $0x4,%eax
          a2++;
 804f2cb:	83 c2 04             	add    $0x4,%edx
  if (!UNALIGNED (s1, s2))
    {  
      /* If s1 and s2 are word-aligned, compare them a word at a time. */
      a1 = (unsigned long*)s1;
      a2 = (unsigned long*)s2;
      while (*a1 == *a2)
 804f2ce:	8b 08                	mov    (%eax),%ecx
 804f2d0:	3b 0a                	cmp    (%edx),%ecx
 804f2d2:	75 e2                	jne    804f2b6 <strcmp+0x22>
        {
          /* To get here, *a1 == *a2, thus if we find a null in *a1,
	     then the strings must be equal, so return zero.  */
          if (DETECTNULL (*a1))
 804f2d4:	8d 99 ff fe fe fe    	lea    -0x1010101(%ecx),%ebx
 804f2da:	f7 d1                	not    %ecx
 804f2dc:	21 cb                	and    %ecx,%ebx
 804f2de:	81 e3 80 80 80 80    	and    $0x80808080,%ebx
 804f2e4:	74 e2                	je     804f2c8 <strcmp+0x34>
	    return 0;
 804f2e6:	31 c0                	xor    %eax,%eax
      s1++;
      s2++;
    }
  return (*(unsigned char *) s1) - (*(unsigned char *) s2);
#endif /* not PREFER_SIZE_OVER_SPEED */
}
 804f2e8:	5b                   	pop    %ebx
 804f2e9:	c3                   	ret    
 804f2ea:	90                   	nop
 804f2eb:	90                   	nop

0804f2ec <__sprint_r>:
int
_DEFUN(__sprint_r, (ptr, fp, uio),
       struct _reent *ptr _AND
       FILE *fp _AND
       register struct __suio *uio)
{
 804f2ec:	55                   	push   %ebp
 804f2ed:	57                   	push   %edi
 804f2ee:	56                   	push   %esi
 804f2ef:	53                   	push   %ebx
 804f2f0:	83 ec 2c             	sub    $0x2c,%esp
 804f2f3:	8b 74 24 40          	mov    0x40(%esp),%esi
 804f2f7:	8b 5c 24 44          	mov    0x44(%esp),%ebx
	register int err = 0;

	if (uio->uio_resid == 0) {
 804f2fb:	8b 54 24 48          	mov    0x48(%esp),%edx
 804f2ff:	8b 42 08             	mov    0x8(%edx),%eax
 804f302:	85 c0                	test   %eax,%eax
 804f304:	0f 84 8a 00 00 00    	je     804f394 <__sprint_r+0xa8>
	if (fp->_flags2 & __SWID) {
		struct __siov *iov;
		wchar_t *p;
		int i, len;

		iov = uio->uio_iov;
 804f30a:	8b 54 24 48          	mov    0x48(%esp),%edx
	if (uio->uio_resid == 0) {
		uio->uio_iovcnt = 0;
		return (0);
	}
#ifdef _WIDE_ORIENT
	if (fp->_flags2 & __SWID) {
 804f30e:	f6 43 65 20          	testb  $0x20,0x65(%ebx)
 804f312:	0f 84 90 00 00 00    	je     804f3a8 <__sprint_r+0xbc>
		struct __siov *iov;
		wchar_t *p;
		int i, len;

		iov = uio->uio_iov;
 804f318:	8b 12                	mov    (%edx),%edx
 804f31a:	89 54 24 1c          	mov    %edx,0x1c(%esp)
		for (; uio->uio_resid != 0;
		     uio->uio_resid -= len * sizeof (wchar_t), iov++) {
			p = (wchar_t *) iov->iov_base;
 804f31e:	8b 54 24 1c          	mov    0x1c(%esp),%edx
 804f322:	8b 3a                	mov    (%edx),%edi
			len = iov->iov_len / sizeof (wchar_t);
 804f324:	8b 42 04             	mov    0x4(%edx),%eax
 804f327:	89 44 24 14          	mov    %eax,0x14(%esp)
			for (i = 0; i < len; i++) {
 804f32b:	31 ed                	xor    %ebp,%ebp
 804f32d:	c1 e8 02             	shr    $0x2,%eax
 804f330:	89 44 24 18          	mov    %eax,0x18(%esp)
 804f334:	75 09                	jne    804f33f <__sprint_r+0x53>
 804f336:	eb 3c                	jmp    804f374 <__sprint_r+0x88>
 804f338:	45                   	inc    %ebp
 804f339:	39 6c 24 18          	cmp    %ebp,0x18(%esp)
 804f33d:	74 35                	je     804f374 <__sprint_r+0x88>
				if (_fputwc_r (ptr, p[i], fp) == WEOF) {
 804f33f:	89 5c 24 08          	mov    %ebx,0x8(%esp)
 804f343:	8b 04 af             	mov    (%edi,%ebp,4),%eax
 804f346:	89 44 24 04          	mov    %eax,0x4(%esp)
 804f34a:	89 34 24             	mov    %esi,(%esp)
 804f34d:	e8 9e 11 00 00       	call   80504f0 <_fputwc_r>
 804f352:	40                   	inc    %eax
 804f353:	75 e3                	jne    804f338 <__sprint_r+0x4c>
					err = -1;
 804f355:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
		}
	} else
#endif
		err = __sfvwrite_r(ptr, fp, uio);
out:
	uio->uio_resid = 0;
 804f35a:	8b 54 24 48          	mov    0x48(%esp),%edx
 804f35e:	c7 42 08 00 00 00 00 	movl   $0x0,0x8(%edx)
	uio->uio_iovcnt = 0;
 804f365:	c7 42 04 00 00 00 00 	movl   $0x0,0x4(%edx)
	return (err);
}
 804f36c:	83 c4 2c             	add    $0x2c,%esp
 804f36f:	5b                   	pop    %ebx
 804f370:	5e                   	pop    %esi
 804f371:	5f                   	pop    %edi
 804f372:	5d                   	pop    %ebp
 804f373:	c3                   	ret    
		wchar_t *p;
		int i, len;

		iov = uio->uio_iov;
		for (; uio->uio_resid != 0;
		     uio->uio_resid -= len * sizeof (wchar_t), iov++) {
 804f374:	8b 7c 24 18          	mov    0x18(%esp),%edi
 804f378:	c1 e7 02             	shl    $0x2,%edi
 804f37b:	8b 54 24 48          	mov    0x48(%esp),%edx
 804f37f:	8b 42 08             	mov    0x8(%edx),%eax
 804f382:	29 f8                	sub    %edi,%eax
 804f384:	89 42 08             	mov    %eax,0x8(%edx)
 804f387:	83 44 24 1c 08       	addl   $0x8,0x1c(%esp)
		struct __siov *iov;
		wchar_t *p;
		int i, len;

		iov = uio->uio_iov;
		for (; uio->uio_resid != 0;
 804f38c:	85 c0                	test   %eax,%eax
 804f38e:	75 8e                	jne    804f31e <__sprint_r+0x32>
 804f390:	eb c8                	jmp    804f35a <__sprint_r+0x6e>
 804f392:	66 90                	xchg   %ax,%ax
       register struct __suio *uio)
{
	register int err = 0;

	if (uio->uio_resid == 0) {
		uio->uio_iovcnt = 0;
 804f394:	c7 42 04 00 00 00 00 	movl   $0x0,0x4(%edx)
		return (0);
 804f39b:	31 c0                	xor    %eax,%eax
		err = __sfvwrite_r(ptr, fp, uio);
out:
	uio->uio_resid = 0;
	uio->uio_iovcnt = 0;
	return (err);
}
 804f39d:	83 c4 2c             	add    $0x2c,%esp
 804f3a0:	5b                   	pop    %ebx
 804f3a1:	5e                   	pop    %esi
 804f3a2:	5f                   	pop    %edi
 804f3a3:	5d                   	pop    %ebp
 804f3a4:	c3                   	ret    
 804f3a5:	8d 76 00             	lea    0x0(%esi),%esi
				}
			}
		}
	} else
#endif
		err = __sfvwrite_r(ptr, fp, uio);
 804f3a8:	89 54 24 08          	mov    %edx,0x8(%esp)
 804f3ac:	89 5c 24 04          	mov    %ebx,0x4(%esp)
 804f3b0:	89 34 24             	mov    %esi,(%esp)
 804f3b3:	e8 a0 d9 ff ff       	call   804cd58 <__sfvwrite_r>
 804f3b8:	eb a0                	jmp    804f35a <__sprint_r+0x6e>
 804f3ba:	66 90                	xchg   %ax,%ax

0804f3bc <_vfiprintf_r>:
_DEFUN(_VFPRINTF_R, (data, fp, fmt0, ap),
       struct _reent *data _AND
       FILE * fp           _AND
       _CONST char *fmt0   _AND
       va_list ap)
{
 804f3bc:	55                   	push   %ebp
 804f3bd:	57                   	push   %edi
 804f3be:	56                   	push   %esi
 804f3bf:	53                   	push   %ebx
 804f3c0:	81 ec ec 00 00 00    	sub    $0xec,%esp
	    (u_long)GET_ARG (N, ap, u_int))
#endif

#ifndef STRING_ONLY
	/* Initialize std streams if not dealing with sprintf family.  */
	CHECK_INIT (data, fp);
 804f3c6:	8b 84 24 00 01 00 00 	mov    0x100(%esp),%eax
 804f3cd:	85 c0                	test   %eax,%eax
 804f3cf:	74 12                	je     804f3e3 <_vfiprintf_r+0x27>
 804f3d1:	8b 84 24 00 01 00 00 	mov    0x100(%esp),%eax
 804f3d8:	8b 68 38             	mov    0x38(%eax),%ebp
 804f3db:	85 ed                	test   %ebp,%ebp
 804f3dd:	0f 84 a7 0b 00 00    	je     804ff8a <_vfiprintf_r+0xbce>
	_newlib_flockfile_start (fp);

	ORIENT(fp, -1);
 804f3e3:	8b 94 24 04 01 00 00 	mov    0x104(%esp),%edx
 804f3ea:	8b 42 0c             	mov    0xc(%edx),%eax
 804f3ed:	f6 c4 20             	test   $0x20,%ah
 804f3f0:	75 0e                	jne    804f400 <_vfiprintf_r+0x44>
 804f3f2:	80 cc 20             	or     $0x20,%ah
 804f3f5:	66 89 42 0c          	mov    %ax,0xc(%edx)
 804f3f9:	81 62 64 ff df ff ff 	andl   $0xffffdfff,0x64(%edx)

	/* sorry, fprintf(read_only_file, "") returns EOF, not 0 */
	if (cantwrite (data, fp)) {
 804f400:	a8 08                	test   $0x8,%al
 804f402:	0f 84 b0 09 00 00    	je     804fdb8 <_vfiprintf_r+0x9fc>
 804f408:	8b 94 24 04 01 00 00 	mov    0x104(%esp),%edx
 804f40f:	8b 7a 10             	mov    0x10(%edx),%edi
 804f412:	85 ff                	test   %edi,%edi
 804f414:	0f 84 9e 09 00 00    	je     804fdb8 <_vfiprintf_r+0x9fc>
		return (EOF);
	}

#ifdef _UNBUF_STREAM_OPT
	/* optimise fprintf(stderr) (and other unbuffered Unix files) */
	if ((fp->_flags & (__SNBF|__SWR|__SRW)) == (__SNBF|__SWR) &&
 804f41a:	83 e0 1a             	and    $0x1a,%eax
 804f41d:	66 83 f8 0a          	cmp    $0xa,%ax
 804f421:	0f 84 ca 09 00 00    	je     804fdf1 <_vfiprintf_r+0xa35>
        }
#endif /* STRING_ONLY */

	fmt = (char *)fmt0;
#ifdef _FVWRITE_IN_STREAMIO
	uio.uio_iov = iovp = iov;
 804f427:	8d 8c 24 a0 00 00 00 	lea    0xa0(%esp),%ecx
 804f42e:	89 4c 24 2c          	mov    %ecx,0x2c(%esp)
 804f432:	89 4c 24 6c          	mov    %ecx,0x6c(%esp)
	uio.uio_resid = 0;
 804f436:	c7 44 24 74 00 00 00 	movl   $0x0,0x74(%esp)
 804f43d:	00 
	uio.uio_iovcnt = 0;
 804f43e:	c7 44 24 70 00 00 00 	movl   $0x0,0x70(%esp)
 804f445:	00 
		}
		fp->_bf._size = 64;
        }
#endif /* STRING_ONLY */

	fmt = (char *)fmt0;
 804f446:	8b ac 24 08 01 00 00 	mov    0x108(%esp),%ebp
#ifdef _FVWRITE_IN_STREAMIO
	uio.uio_iov = iovp = iov;
 804f44d:	89 cf                	mov    %ecx,%edi
	u_quad_t _uquad;	/* integer arguments %[diouxX] */
	enum { OCT, DEC, HEX } base;/* base for [diouxX] conversion */
	int dprec;		/* a copy of prec if [diouxX], 0 otherwise */
	int realsz;		/* field size expanded by dprec */
	int size;		/* size of converted field or string */
	char *xdigs = NULL;	/* digits for [xX] conversion */
 804f44f:	c7 44 24 50 00 00 00 	movl   $0x0,0x50(%esp)
 804f456:	00 
#ifdef _FVWRITE_IN_STREAMIO
	uio.uio_iov = iovp = iov;
	uio.uio_resid = 0;
	uio.uio_iovcnt = 0;
#endif
	ret = 0;
 804f457:	c7 44 24 44 00 00 00 	movl   $0x0,0x44(%esp)
 804f45e:	00 
			*
			* To demonstrate this case, compile and run:
                        *    printf ("%#.0o",0);
			*/
                       else if (base == OCT && (flags & ALT))
                         *--cp = '0';
 804f45f:	8d 8c 24 9f 00 00 00 	lea    0x9f(%esp),%ecx
 804f466:	89 4c 24 28          	mov    %ecx,0x28(%esp)
 804f46a:	89 f8                	mov    %edi,%eax
 804f46c:	29 c8                	sub    %ecx,%eax
 804f46e:	89 44 24 5c          	mov    %eax,0x5c(%esp)
 804f472:	89 f9                	mov    %edi,%ecx
                    else if (wc == '%')
                        break;
                    fmt += n;
		}
#else
                while (*fmt != '\0' && *fmt != '%')
 804f474:	8a 45 00             	mov    0x0(%ebp),%al
 804f477:	89 eb                	mov    %ebp,%ebx
 804f479:	84 c0                	test   %al,%al
 804f47b:	74 3c                	je     804f4b9 <_vfiprintf_r+0xfd>
 804f47d:	3c 25                	cmp    $0x25,%al
 804f47f:	75 07                	jne    804f488 <_vfiprintf_r+0xcc>
 804f481:	eb 36                	jmp    804f4b9 <_vfiprintf_r+0xfd>
 804f483:	90                   	nop
 804f484:	3c 25                	cmp    $0x25,%al
 804f486:	74 07                	je     804f48f <_vfiprintf_r+0xd3>
                    fmt += 1;
 804f488:	43                   	inc    %ebx
                    else if (wc == '%')
                        break;
                    fmt += n;
		}
#else
                while (*fmt != '\0' && *fmt != '%')
 804f489:	8a 03                	mov    (%ebx),%al
 804f48b:	84 c0                	test   %al,%al
 804f48d:	75 f5                	jne    804f484 <_vfiprintf_r+0xc8>
 804f48f:	89 de                	mov    %ebx,%esi
 804f491:	29 ee                	sub    %ebp,%esi
                    fmt += 1;
#endif
		if ((m = fmt - cp) != 0) {
 804f493:	85 f6                	test   %esi,%esi
 804f495:	74 22                	je     804f4b9 <_vfiprintf_r+0xfd>
			PRINT (cp, m);
 804f497:	89 29                	mov    %ebp,(%ecx)
 804f499:	89 71 04             	mov    %esi,0x4(%ecx)
 804f49c:	01 74 24 74          	add    %esi,0x74(%esp)
 804f4a0:	8b 44 24 70          	mov    0x70(%esp),%eax
 804f4a4:	40                   	inc    %eax
 804f4a5:	89 44 24 70          	mov    %eax,0x70(%esp)
 804f4a9:	83 f8 07             	cmp    $0x7,%eax
 804f4ac:	0f 8f 82 09 00 00    	jg     804fe34 <_vfiprintf_r+0xa78>
 804f4b2:	83 c1 08             	add    $0x8,%ecx
			ret += m;
 804f4b5:	01 74 24 44          	add    %esi,0x44(%esp)
		}
#ifdef _MB_CAPABLE
		if (n <= 0)
                    goto done;
#else
                if (*fmt == '\0')
 804f4b9:	80 3b 00             	cmpb   $0x0,(%ebx)
 804f4bc:	0f 84 8a 07 00 00    	je     804fc4c <_vfiprintf_r+0x890>
                    goto done;
#endif
		fmt_anchor = fmt;
		fmt++;		/* skip over '%' */
 804f4c2:	8d 6b 01             	lea    0x1(%ebx),%ebp

		flags = 0;
		dprec = 0;
		width = 0;
		prec = -1;
		sign = '\0';
 804f4c5:	c6 44 24 69 00       	movb   $0x0,0x69(%esp)
 804f4ca:	31 f6                	xor    %esi,%esi
		fmt++;		/* skip over '%' */

		flags = 0;
		dprec = 0;
		width = 0;
		prec = -1;
 804f4cc:	bf ff ff ff ff       	mov    $0xffffffff,%edi
		fmt_anchor = fmt;
		fmt++;		/* skip over '%' */

		flags = 0;
		dprec = 0;
		width = 0;
 804f4d1:	c7 44 24 48 00 00 00 	movl   $0x0,0x48(%esp)
 804f4d8:	00 
                    goto done;
#endif
		fmt_anchor = fmt;
		fmt++;		/* skip over '%' */

		flags = 0;
 804f4d9:	c7 44 24 30 00 00 00 	movl   $0x0,0x30(%esp)
 804f4e0:	00 
#ifndef _NO_POS_ARGS
		N = arg_index;
		is_pos_arg = 0;
#endif

rflag:		ch = *fmt++;
 804f4e1:	0f be 45 00          	movsbl 0x0(%ebp),%eax
 804f4e5:	45                   	inc    %ebp
reswitch:	switch (ch) {
 804f4e6:	8d 50 e0             	lea    -0x20(%eax),%edx
 804f4e9:	83 fa 58             	cmp    $0x58,%edx
 804f4ec:	0f 86 12 01 00 00    	jbe    804f604 <_vfiprintf_r+0x248>
 804f4f2:	89 f2                	mov    %esi,%edx
 804f4f4:	88 54 24 69          	mov    %dl,0x69(%esp)

			size = buf + BUF - cp;
		skipsize:
			break;
		default:	/* "%?" prints ?, unless ? is NUL */
			if (ch == '\0')
 804f4f8:	85 c0                	test   %eax,%eax
 804f4fa:	0f 84 4c 07 00 00    	je     804fc4c <_vfiprintf_r+0x890>
				goto done;
			/* pretend it was %c with argument ch */
			cp = buf;
			*cp = ch;
 804f500:	88 44 24 78          	mov    %al,0x78(%esp)
			size = 1;
			sign = '\0';
 804f504:	c6 44 24 69 00       	movb   $0x0,0x69(%esp)
			break;
 804f509:	c7 44 24 34 01 00 00 	movl   $0x1,0x34(%esp)
 804f510:	00 
			if (ch == '\0')
				goto done;
			/* pretend it was %c with argument ch */
			cp = buf;
			*cp = ch;
			size = 1;
 804f511:	c7 44 24 3c 01 00 00 	movl   $0x1,0x3c(%esp)
 804f518:	00 
			break;
		default:	/* "%?" prints ?, unless ? is NUL */
			if (ch == '\0')
				goto done;
			/* pretend it was %c with argument ch */
			cp = buf;
 804f519:	8d 5c 24 78          	lea    0x78(%esp),%ebx
#endif
		fmt_anchor = fmt;
		fmt++;		/* skip over '%' */

		flags = 0;
		dprec = 0;
 804f51d:	c7 44 24 40 00 00 00 	movl   $0x0,0x40(%esp)
 804f524:	00 
 804f525:	8d 76 00             	lea    0x0(%esi),%esi
		 * size excludes decimal prec; realsz includes it.
		 */
		realsz = dprec > size ? dprec : size;
		if (sign)
			realsz++;
		if (flags & HEXPREFIX)
 804f528:	8b 44 24 30          	mov    0x30(%esp),%eax
 804f52c:	83 e0 02             	and    $0x2,%eax
 804f52f:	89 44 24 38          	mov    %eax,0x38(%esp)
 804f533:	74 05                	je     804f53a <_vfiprintf_r+0x17e>
			realsz+= 2;
 804f535:	83 44 24 34 02       	addl   $0x2,0x34(%esp)

		/* right-adjusting blank padding */
		if ((flags & (LADJUST|ZEROPAD)) == 0)
 804f53a:	8b 54 24 30          	mov    0x30(%esp),%edx
 804f53e:	81 e2 84 00 00 00    	and    $0x84,%edx
 804f544:	89 54 24 4c          	mov    %edx,0x4c(%esp)
 804f548:	0f 85 72 04 00 00    	jne    804f9c0 <_vfiprintf_r+0x604>
			PAD (width - realsz, blanks);
 804f54e:	8b 7c 24 48          	mov    0x48(%esp),%edi
 804f552:	2b 7c 24 34          	sub    0x34(%esp),%edi
 804f556:	85 ff                	test   %edi,%edi
 804f558:	0f 8e 62 04 00 00    	jle    804f9c0 <_vfiprintf_r+0x604>
 804f55e:	8b 44 24 70          	mov    0x70(%esp),%eax
 804f562:	83 ff 10             	cmp    $0x10,%edi
 804f565:	7e 77                	jle    804f5de <_vfiprintf_r+0x222>
 804f567:	8d 74 24 6c          	lea    0x6c(%esp),%esi
 804f56b:	89 6c 24 54          	mov    %ebp,0x54(%esp)
 804f56f:	89 5c 24 58          	mov    %ebx,0x58(%esp)
 804f573:	8b 9c 24 00 01 00 00 	mov    0x100(%esp),%ebx
 804f57a:	8b ac 24 04 01 00 00 	mov    0x104(%esp),%ebp
 804f581:	eb 0c                	jmp    804f58f <_vfiprintf_r+0x1d3>
 804f583:	90                   	nop
 804f584:	83 c1 08             	add    $0x8,%ecx
 804f587:	83 ef 10             	sub    $0x10,%edi
 804f58a:	83 ff 10             	cmp    $0x10,%edi
 804f58d:	7e 47                	jle    804f5d6 <_vfiprintf_r+0x21a>
 804f58f:	c7 01 58 1b 05 08    	movl   $0x8051b58,(%ecx)
 804f595:	c7 41 04 10 00 00 00 	movl   $0x10,0x4(%ecx)
 804f59c:	83 44 24 74 10       	addl   $0x10,0x74(%esp)
 804f5a1:	40                   	inc    %eax
 804f5a2:	89 44 24 70          	mov    %eax,0x70(%esp)
 804f5a6:	83 f8 07             	cmp    $0x7,%eax
 804f5a9:	7e d9                	jle    804f584 <_vfiprintf_r+0x1c8>
 804f5ab:	89 74 24 08          	mov    %esi,0x8(%esp)
 804f5af:	89 6c 24 04          	mov    %ebp,0x4(%esp)
 804f5b3:	89 1c 24             	mov    %ebx,(%esp)
 804f5b6:	e8 31 fd ff ff       	call   804f2ec <__sprint_r>
 804f5bb:	85 c0                	test   %eax,%eax
 804f5bd:	0f 85 b5 06 00 00    	jne    804fc78 <_vfiprintf_r+0x8bc>
 804f5c3:	8d 8c 24 a0 00 00 00 	lea    0xa0(%esp),%ecx
 804f5ca:	8b 44 24 70          	mov    0x70(%esp),%eax
 804f5ce:	83 ef 10             	sub    $0x10,%edi
 804f5d1:	83 ff 10             	cmp    $0x10,%edi
 804f5d4:	7f b9                	jg     804f58f <_vfiprintf_r+0x1d3>
 804f5d6:	8b 6c 24 54          	mov    0x54(%esp),%ebp
 804f5da:	8b 5c 24 58          	mov    0x58(%esp),%ebx
 804f5de:	c7 01 58 1b 05 08    	movl   $0x8051b58,(%ecx)
 804f5e4:	89 79 04             	mov    %edi,0x4(%ecx)
 804f5e7:	01 7c 24 74          	add    %edi,0x74(%esp)
 804f5eb:	40                   	inc    %eax
 804f5ec:	89 44 24 70          	mov    %eax,0x70(%esp)
 804f5f0:	83 f8 07             	cmp    $0x7,%eax
 804f5f3:	0f 8f 94 03 00 00    	jg     804f98d <_vfiprintf_r+0x5d1>
 804f5f9:	83 c1 08             	add    $0x8,%ecx
 804f5fc:	e9 c3 03 00 00       	jmp    804f9c4 <_vfiprintf_r+0x608>
 804f601:	8d 76 00             	lea    0x0(%esi),%esi
		N = arg_index;
		is_pos_arg = 0;
#endif

rflag:		ch = *fmt++;
reswitch:	switch (ch) {
 804f604:	ff 24 95 f4 19 05 08 	jmp    *0x80519f4(,%edx,4)
			} else
#endif
				flags |= LONGINT;
			goto rflag;
		case 'q': /* extension */
			flags |= QUADINT;
 804f60b:	83 4c 24 30 10       	orl    $0x10,0x30(%esp)
			goto rflag;
 804f610:	e9 cc fe ff ff       	jmp    804f4e1 <_vfiprintf_r+0x125>
#endif
			else
				*GET_ARG (N, ap, int_ptr_t) = ret;
			continue;	/* no output */
		case 'O': /* extension */
			flags |= LONGINT;
 804f615:	83 4c 24 30 10       	orl    $0x10,0x30(%esp)
			/*FALLTHROUGH*/
		case 'o':
			_uquad = UARG ();
 804f61a:	f6 44 24 30 10       	testb  $0x10,0x30(%esp)
 804f61f:	0f 84 72 09 00 00    	je     804ff97 <_vfiprintf_r+0xbdb>
 804f625:	8b 94 24 0c 01 00 00 	mov    0x10c(%esp),%edx
 804f62c:	8b 32                	mov    (%edx),%esi
 804f62e:	83 c2 04             	add    $0x4,%edx
 804f631:	89 94 24 0c 01 00 00 	mov    %edx,0x10c(%esp)
			base = OCT;
 804f638:	31 c0                	xor    %eax,%eax
 804f63a:	66 90                	xchg   %ax,%ax

#ifdef _WANT_IO_C99_FORMATS
			flags &= ~GROUPING;
#endif
			/* unsigned conversions */
nosign:			sign = '\0';
 804f63c:	c6 44 24 69 00       	movb   $0x0,0x69(%esp)
 804f641:	c6 44 24 38 00       	movb   $0x0,0x38(%esp)
			/*
			 * ``... diouXx conversions ... if a precision is
			 * specified, the 0 flag will be ignored.''
			 *	-- ANSI X3J11
			 */
number:			if ((dprec = prec) >= 0)
 804f646:	85 ff                	test   %edi,%edi
 804f648:	78 08                	js     804f652 <_vfiprintf_r+0x296>
				flags &= ~ZEROPAD;
 804f64a:	81 64 24 30 7f ff ff 	andl   $0xffffff7f,0x30(%esp)
 804f651:	ff 
			 * ``The result of converting a zero value with an
			 * explicit precision of zero is no characters.''
			 *	-- ANSI X3J11
			 */
			cp = buf + BUF;
			if (_uquad != 0 || prec != 0) {
 804f652:	85 f6                	test   %esi,%esi
 804f654:	0f 85 a6 05 00 00    	jne    804fc00 <_vfiprintf_r+0x844>
 804f65a:	85 ff                	test   %edi,%edi
 804f65c:	0f 85 9e 05 00 00    	jne    804fc00 <_vfiprintf_r+0x844>
			*     -- ANSI X3J11
			*
			* To demonstrate this case, compile and run:
                        *    printf ("%#.0o",0);
			*/
                       else if (base == OCT && (flags & ALT))
 804f662:	84 c0                	test   %al,%al
 804f664:	75 0b                	jne    804f671 <_vfiprintf_r+0x2b5>
 804f666:	f6 44 24 30 01       	testb  $0x1,0x30(%esp)
 804f66b:	0f 85 ae 09 00 00    	jne    805001f <_vfiprintf_r+0xc63>
 804f671:	c7 44 24 3c 00 00 00 	movl   $0x0,0x3c(%esp)
 804f678:	00 
			/*
			 * ``The result of converting a zero value with an
			 * explicit precision of zero is no characters.''
			 *	-- ANSI X3J11
			 */
			cp = buf + BUF;
 804f679:	8d 9c 24 a0 00 00 00 	lea    0xa0(%esp),%ebx
#endif
		fmt_anchor = fmt;
		fmt++;		/* skip over '%' */

		flags = 0;
		dprec = 0;
 804f680:	89 7c 24 40          	mov    %edi,0x40(%esp)
		 * If flags&FPT, ch must be in [aAeEfg].
		 *
		 * Compute actual size, so we know how much to pad.
		 * size excludes decimal prec; realsz includes it.
		 */
		realsz = dprec > size ? dprec : size;
 804f684:	8b 44 24 3c          	mov    0x3c(%esp),%eax
 804f688:	89 44 24 34          	mov    %eax,0x34(%esp)
 804f68c:	8b 54 24 40          	mov    0x40(%esp),%edx
 804f690:	39 d0                	cmp    %edx,%eax
 804f692:	7d 04                	jge    804f698 <_vfiprintf_r+0x2dc>
 804f694:	89 54 24 34          	mov    %edx,0x34(%esp)
		if (sign)
			realsz++;
 804f698:	80 7c 24 38 01       	cmpb   $0x1,0x38(%esp)
 804f69d:	83 5c 24 34 ff       	sbbl   $0xffffffff,0x34(%esp)
 804f6a2:	e9 81 fe ff ff       	jmp    804f528 <_vfiprintf_r+0x16c>
			} else
				size = strlen (cp);

			break;
		case 'U': /* extension */
			flags |= LONGINT;
 804f6a7:	83 4c 24 30 10       	orl    $0x10,0x30(%esp)
			/*FALLTHROUGH*/
		case 'u':
			_uquad = UARG ();
 804f6ac:	f6 44 24 30 10       	testb  $0x10,0x30(%esp)
 804f6b1:	0f 84 0f 09 00 00    	je     804ffc6 <_vfiprintf_r+0xc0a>
 804f6b7:	8b 84 24 0c 01 00 00 	mov    0x10c(%esp),%eax
 804f6be:	8b 30                	mov    (%eax),%esi
 804f6c0:	83 c0 04             	add    $0x4,%eax
 804f6c3:	89 84 24 0c 01 00 00 	mov    %eax,0x10c(%esp)
			base = DEC;
 804f6ca:	b0 01                	mov    $0x1,%al
 804f6cc:	e9 6b ff ff ff       	jmp    804f63c <_vfiprintf_r+0x280>
			 * ``A negative field width argument is taken as a
			 * - flag followed by a positive field width.''
			 *	-- ANSI X3J11
			 * They don't exclude field widths read from args.
			 */
			width = GET_ARG (n, ap, int);
 804f6d1:	8b 84 24 0c 01 00 00 	mov    0x10c(%esp),%eax
 804f6d8:	83 c0 04             	add    $0x4,%eax
 804f6db:	8b 94 24 0c 01 00 00 	mov    0x10c(%esp),%edx
 804f6e2:	8b 12                	mov    (%edx),%edx
 804f6e4:	89 54 24 48          	mov    %edx,0x48(%esp)
#ifndef _NO_POS_ARGS
			is_pos_arg = old_is_pos_arg;
#endif
			if (width >= 0)
 804f6e8:	85 d2                	test   %edx,%edx
 804f6ea:	0f 89 23 09 00 00    	jns    8050013 <_vfiprintf_r+0xc57>
				goto rflag;
			width = -width;
 804f6f0:	f7 da                	neg    %edx
 804f6f2:	89 54 24 48          	mov    %edx,0x48(%esp)
			 * ``A negative field width argument is taken as a
			 * - flag followed by a positive field width.''
			 *	-- ANSI X3J11
			 * They don't exclude field widths read from args.
			 */
			width = GET_ARG (n, ap, int);
 804f6f6:	89 84 24 0c 01 00 00 	mov    %eax,0x10c(%esp)
			if (width >= 0)
				goto rflag;
			width = -width;
			/* FALLTHROUGH */
		case '-':
			flags |= LADJUST;
 804f6fd:	83 4c 24 30 04       	orl    $0x4,0x30(%esp)
			goto rflag;
 804f702:	e9 da fd ff ff       	jmp    804f4e1 <_vfiprintf_r+0x125>
 804f707:	89 f2                	mov    %esi,%edx
 804f709:	88 54 24 69          	mov    %dl,0x69(%esp)
#ifndef _NO_LONGLONG
			if (flags & QUADINT)
				*GET_ARG (N, ap, quad_ptr_t) = ret;
			else
#endif
			if (flags & LONGINT)
 804f70d:	f6 44 24 30 10       	testb  $0x10,0x30(%esp)
 804f712:	0f 85 23 09 00 00    	jne    805003b <_vfiprintf_r+0xc7f>
				*GET_ARG (N, ap, long_ptr_t) = ret;
			else if (flags & SHORTINT)
 804f718:	f6 44 24 30 40       	testb  $0x40,0x30(%esp)
 804f71d:	0f 84 18 09 00 00    	je     805003b <_vfiprintf_r+0xc7f>
				*GET_ARG (N, ap, short_ptr_t) = ret;
 804f723:	8b 94 24 0c 01 00 00 	mov    0x10c(%esp),%edx
 804f72a:	8b 02                	mov    (%edx),%eax
 804f72c:	8b 54 24 44          	mov    0x44(%esp),%edx
 804f730:	66 89 10             	mov    %dx,(%eax)
 804f733:	8b 84 24 0c 01 00 00 	mov    0x10c(%esp),%eax
 804f73a:	83 c0 04             	add    $0x4,%eax
 804f73d:	89 84 24 0c 01 00 00 	mov    %eax,0x10c(%esp)
 804f744:	e9 2b fd ff ff       	jmp    804f474 <_vfiprintf_r+0xb8>
 804f749:	89 f2                	mov    %esi,%edx
 804f74b:	88 54 24 69          	mov    %dl,0x69(%esp)
			goto nosign;
		case 'X':
			xdigs = "0123456789ABCDEF";
			goto hex;
		case 'x':
			xdigs = "0123456789abcdef";
 804f74f:	c7 44 24 50 b5 16 05 	movl   $0x80516b5,0x50(%esp)
 804f756:	08 
hex:			_uquad = UARG ();
 804f757:	8b 94 24 0c 01 00 00 	mov    0x10c(%esp),%edx
 804f75e:	f6 44 24 30 10       	testb  $0x10,0x30(%esp)
 804f763:	75 0b                	jne    804f770 <_vfiprintf_r+0x3b4>
 804f765:	f6 44 24 30 40       	testb  $0x40,0x30(%esp)
 804f76a:	0f 85 03 09 00 00    	jne    8050073 <_vfiprintf_r+0xcb7>
 804f770:	8b 32                	mov    (%edx),%esi
 804f772:	83 c2 04             	add    $0x4,%edx
 804f775:	89 94 24 0c 01 00 00 	mov    %edx,0x10c(%esp)
			base = HEX;
			/* leading 0x/X only if non-zero */
			if (flags & ALT && _uquad != 0) {
 804f77c:	f6 44 24 30 01       	testb  $0x1,0x30(%esp)
 804f781:	74 19                	je     804f79c <_vfiprintf_r+0x3e0>
 804f783:	85 f6                	test   %esi,%esi
 804f785:	74 15                	je     804f79c <_vfiprintf_r+0x3e0>
				ox[0] = '0';
 804f787:	c6 44 24 6a 30       	movb   $0x30,0x6a(%esp)
				ox[1] = ch;
 804f78c:	88 44 24 6b          	mov    %al,0x6b(%esp)
				flags |= HEXPREFIX;
 804f790:	83 4c 24 30 02       	orl    $0x2,0x30(%esp)
			xdigs = "0123456789ABCDEF";
			goto hex;
		case 'x':
			xdigs = "0123456789abcdef";
hex:			_uquad = UARG ();
			base = HEX;
 804f795:	b0 02                	mov    $0x2,%al
 804f797:	e9 a0 fe ff ff       	jmp    804f63c <_vfiprintf_r+0x280>
 804f79c:	b0 02                	mov    $0x2,%al
 804f79e:	e9 99 fe ff ff       	jmp    804f63c <_vfiprintf_r+0x280>
			if (*fmt == 'h') {
				fmt++;
				flags |= CHARINT;
			} else
#endif
				flags |= SHORTINT;
 804f7a3:	83 4c 24 30 40       	orl    $0x40,0x30(%esp)
			goto rflag;
 804f7a8:	e9 34 fd ff ff       	jmp    804f4e1 <_vfiprintf_r+0x125>
 804f7ad:	89 f2                	mov    %esi,%edx
 804f7af:	88 54 24 69          	mov    %dl,0x69(%esp)
		case 'D':  /* extension */
			flags |= LONGINT;
			/*FALLTHROUGH*/
		case 'd':
		case 'i':
			_uquad = SARG ();
 804f7b3:	f6 44 24 30 10       	testb  $0x10,0x30(%esp)
 804f7b8:	75 0b                	jne    804f7c5 <_vfiprintf_r+0x409>
 804f7ba:	f6 44 24 30 40       	testb  $0x40,0x30(%esp)
 804f7bf:	0f 85 dc 08 00 00    	jne    80500a1 <_vfiprintf_r+0xce5>
 804f7c5:	8b 94 24 0c 01 00 00 	mov    0x10c(%esp),%edx
 804f7cc:	8b 32                	mov    (%edx),%esi
 804f7ce:	83 c2 04             	add    $0x4,%edx
 804f7d1:	89 94 24 0c 01 00 00 	mov    %edx,0x10c(%esp)
#ifndef _NO_LONGLONG
			if ((quad_t)_uquad < 0)
#else
			if ((long) _uquad < 0)
 804f7d8:	85 f6                	test   %esi,%esi
 804f7da:	0f 88 dd 08 00 00    	js     80500bd <_vfiprintf_r+0xd01>
 804f7e0:	8a 44 24 69          	mov    0x69(%esp),%al
 804f7e4:	88 44 24 38          	mov    %al,0x38(%esp)
			{

				_uquad = -_uquad;
				sign = '-';
			}
			base = DEC;
 804f7e8:	b0 01                	mov    $0x1,%al
 804f7ea:	e9 57 fe ff ff       	jmp    804f646 <_vfiprintf_r+0x28a>
				}
			}
			else
#endif /* _MB_CAPABLE */
			{
				*cp = GET_ARG (N, ap, int);
 804f7ef:	8b 94 24 0c 01 00 00 	mov    0x10c(%esp),%edx
 804f7f6:	8b 02                	mov    (%edx),%eax
 804f7f8:	88 44 24 78          	mov    %al,0x78(%esp)
				size = 1;
			}
			sign = '\0';
 804f7fc:	c6 44 24 69 00       	movb   $0x0,0x69(%esp)
				}
			}
			else
#endif /* _MB_CAPABLE */
			{
				*cp = GET_ARG (N, ap, int);
 804f801:	83 c2 04             	add    $0x4,%edx
 804f804:	89 94 24 0c 01 00 00 	mov    %edx,0x10c(%esp)
				size = 1;
			}
			sign = '\0';
			break;
 804f80b:	c7 44 24 34 01 00 00 	movl   $0x1,0x34(%esp)
 804f812:	00 
			}
			else
#endif /* _MB_CAPABLE */
			{
				*cp = GET_ARG (N, ap, int);
				size = 1;
 804f813:	c7 44 24 3c 01 00 00 	movl   $0x1,0x3c(%esp)
 804f81a:	00 
		    flags |= QUADINT;
		  goto rflag;
		case 'C':
#endif /* _WANT_IO_C99_FORMATS */
		case 'c':
			cp = buf;
 804f81b:	8d 5c 24 78          	lea    0x78(%esp),%ebx
			{
				*cp = GET_ARG (N, ap, int);
				size = 1;
			}
			sign = '\0';
			break;
 804f81f:	e9 f9 fc ff ff       	jmp    804f51d <_vfiprintf_r+0x161>
 804f824:	89 f2                	mov    %esi,%edx
 804f826:	88 54 24 69          	mov    %dl,0x69(%esp)
		case 'u':
			_uquad = UARG ();
			base = DEC;
			goto nosign;
		case 'X':
			xdigs = "0123456789ABCDEF";
 804f82a:	c7 44 24 50 a4 16 05 	movl   $0x80516a4,0x50(%esp)
 804f831:	08 
 804f832:	e9 20 ff ff ff       	jmp    804f757 <_vfiprintf_r+0x39b>
 804f837:	89 f0                	mov    %esi,%eax
 804f839:	88 44 24 69          	mov    %al,0x69(%esp)
				size = 1;
			}
			sign = '\0';
			break;
		case 'D':  /* extension */
			flags |= LONGINT;
 804f83d:	83 4c 24 30 10       	orl    $0x10,0x30(%esp)
 804f842:	e9 6c ff ff ff       	jmp    804f7b3 <_vfiprintf_r+0x3f7>
			goto nosign;
		case 's':
#ifdef _WANT_IO_C99_FORMATS
		case 'S':
#endif
			cp = GET_ARG (N, ap, char_ptr_t);
 804f847:	8b b4 24 0c 01 00 00 	mov    0x10c(%esp),%esi
 804f84e:	83 c6 04             	add    $0x4,%esi
 804f851:	8b 84 24 0c 01 00 00 	mov    0x10c(%esp),%eax
 804f858:	8b 18                	mov    (%eax),%ebx
#ifdef _GLIBC_EXTENSION
string:
#endif
			sign = '\0';
 804f85a:	c6 44 24 69 00       	movb   $0x0,0x69(%esp)
#ifndef __OPTIMIZE_SIZE__
			/* Behavior is undefined if the user passed a
			   NULL string when precision is not 0.
			   However, if we are not optimizing for size,
			   we might as well mirror glibc behavior.  */
			if (cp == NULL) {
 804f85f:	85 db                	test   %ebx,%ebx
 804f861:	0f 84 bf 08 00 00    	je     8050126 <_vfiprintf_r+0xd6a>
				}
				cp[size] = '\0';
			}
			else
#endif /* _MB_CAPABLE */
			if (prec >= 0) {
 804f867:	85 ff                	test   %edi,%edi
 804f869:	0f 88 e5 08 00 00    	js     8050154 <_vfiprintf_r+0xd98>
				/*
				 * can't use strlen; can only look for the
				 * NUL in the first `prec' characters, and
				 * strlen () will go further.
				 */
				char *p = memchr (cp, 0, prec);
 804f86f:	89 7c 24 08          	mov    %edi,0x8(%esp)
 804f873:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
 804f87a:	00 
 804f87b:	89 1c 24             	mov    %ebx,(%esp)
 804f87e:	89 4c 24 18          	mov    %ecx,0x18(%esp)
 804f882:	e8 29 e3 ff ff       	call   804dbb0 <memchr>

				if (p != NULL) {
 804f887:	85 c0                	test   %eax,%eax
 804f889:	8b 4c 24 18          	mov    0x18(%esp),%ecx
 804f88d:	0f 84 f1 08 00 00    	je     8050184 <_vfiprintf_r+0xdc8>
					size = p - cp;
 804f893:	29 d8                	sub    %ebx,%eax
 804f895:	89 44 24 3c          	mov    %eax,0x3c(%esp)
					if (size > prec)
 804f899:	39 f8                	cmp    %edi,%eax
 804f89b:	0f 8e 69 08 00 00    	jle    805010a <_vfiprintf_r+0xd4e>
 804f8a1:	8a 54 24 69          	mov    0x69(%esp),%dl
 804f8a5:	88 54 24 38          	mov    %dl,0x38(%esp)
 804f8a9:	89 7c 24 3c          	mov    %edi,0x3c(%esp)
			goto nosign;
		case 's':
#ifdef _WANT_IO_C99_FORMATS
		case 'S':
#endif
			cp = GET_ARG (N, ap, char_ptr_t);
 804f8ad:	89 b4 24 0c 01 00 00 	mov    %esi,0x10c(%esp)
#endif
		fmt_anchor = fmt;
		fmt++;		/* skip over '%' */

		flags = 0;
		dprec = 0;
 804f8b4:	c7 44 24 40 00 00 00 	movl   $0x0,0x40(%esp)
 804f8bb:	00 
 804f8bc:	e9 c3 fd ff ff       	jmp    804f684 <_vfiprintf_r+0x2c8>
			 * of printable characters, in an implementation-
			 * defined manner.''
			 *	-- ANSI X3J11
			 */
			/* NOSTRICT */
			_uquad = (uintptr_t) GET_ARG (N, ap, void_ptr_t);
 804f8c1:	8b 94 24 0c 01 00 00 	mov    0x10c(%esp),%edx
 804f8c8:	8b 32                	mov    (%edx),%esi
			base = HEX;
			xdigs = "0123456789abcdef";
			flags |= HEXPREFIX;
 804f8ca:	83 4c 24 30 02       	orl    $0x2,0x30(%esp)
			ox[0] = '0';
 804f8cf:	c6 44 24 6a 30       	movb   $0x30,0x6a(%esp)
			ox[1] = ch = 'x';
 804f8d4:	c6 44 24 6b 78       	movb   $0x78,0x6b(%esp)
			 * of printable characters, in an implementation-
			 * defined manner.''
			 *	-- ANSI X3J11
			 */
			/* NOSTRICT */
			_uquad = (uintptr_t) GET_ARG (N, ap, void_ptr_t);
 804f8d9:	83 c2 04             	add    $0x4,%edx
 804f8dc:	89 94 24 0c 01 00 00 	mov    %edx,0x10c(%esp)
			base = HEX;
			xdigs = "0123456789abcdef";
 804f8e3:	c7 44 24 50 b5 16 05 	movl   $0x80516b5,0x50(%esp)
 804f8ea:	08 
			 * defined manner.''
			 *	-- ANSI X3J11
			 */
			/* NOSTRICT */
			_uquad = (uintptr_t) GET_ARG (N, ap, void_ptr_t);
			base = HEX;
 804f8eb:	b0 02                	mov    $0x2,%al
			xdigs = "0123456789abcdef";
			flags |= HEXPREFIX;
			ox[0] = '0';
			ox[1] = ch = 'x';
			goto nosign;
 804f8ed:	e9 4a fd ff ff       	jmp    804f63c <_vfiprintf_r+0x280>
			/* FALLTHROUGH */
		case '-':
			flags |= LADJUST;
			goto rflag;
		case '+':
			sign = '+';
 804f8f2:	be 2b 00 00 00       	mov    $0x2b,%esi
 804f8f7:	e9 e5 fb ff ff       	jmp    804f4e1 <_vfiprintf_r+0x125>
			 */
			if (!sign)
				sign = ' ';
			goto rflag;
		case '#':
			flags |= ALT;
 804f8fc:	83 4c 24 30 01       	orl    $0x1,0x30(%esp)
			goto rflag;
 804f901:	e9 db fb ff ff       	jmp    804f4e1 <_vfiprintf_r+0x125>
			/*
			 * ``If the space and + flags both appear, the space
			 * flag will be ignored.''
			 *	-- ANSI X3J11
			 */
			if (!sign)
 804f906:	89 f0                	mov    %esi,%eax
 804f908:	84 c0                	test   %al,%al
 804f90a:	0f 85 d1 fb ff ff    	jne    804f4e1 <_vfiprintf_r+0x125>
				sign = ' ';
 804f910:	be 20 00 00 00       	mov    $0x20,%esi
 804f915:	e9 c7 fb ff ff       	jmp    804f4e1 <_vfiprintf_r+0x125>
		N = arg_index;
		is_pos_arg = 0;
#endif

rflag:		ch = *fmt++;
reswitch:	switch (ch) {
 804f91a:	31 db                	xor    %ebx,%ebx
 804f91c:	8d 50 d0             	lea    -0x30(%eax),%edx
 804f91f:	90                   	nop
			goto rflag;
		case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
			n = 0;
			do {
				n = 10 * n + to_digit (ch);
 804f920:	8d 04 9b             	lea    (%ebx,%ebx,4),%eax
 804f923:	8d 1c 42             	lea    (%edx,%eax,2),%ebx
				ch = *fmt++;
 804f926:	0f be 45 00          	movsbl 0x0(%ebp),%eax
 804f92a:	45                   	inc    %ebp
			} while (is_digit (ch));
 804f92b:	8d 50 d0             	lea    -0x30(%eax),%edx
 804f92e:	83 fa 09             	cmp    $0x9,%edx
 804f931:	76 ed                	jbe    804f920 <_vfiprintf_r+0x564>
			goto rflag;
		case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
			n = 0;
			do {
				n = 10 * n + to_digit (ch);
 804f933:	89 5c 24 48          	mov    %ebx,0x48(%esp)
 804f937:	e9 aa fb ff ff       	jmp    804f4e6 <_vfiprintf_r+0x12a>
			/*
			 * ``Note that 0 is taken as a flag, not as the
			 * beginning of a field width.''
			 *	-- ANSI X3J11
			 */
			flags |= ZEROPAD;
 804f93c:	81 4c 24 30 80 00 00 	orl    $0x80,0x30(%esp)
 804f943:	00 
			goto rflag;
 804f944:	e9 98 fb ff ff       	jmp    804f4e1 <_vfiprintf_r+0x125>
			goto rflag;
		case '+':
			sign = '+';
			goto rflag;
		case '.':
			if ((ch = *fmt++) == '*') {
 804f949:	0f be 45 00          	movsbl 0x0(%ebp),%eax
 804f94d:	45                   	inc    %ebp
 804f94e:	83 f8 2a             	cmp    $0x2a,%eax
 804f951:	0f 84 57 08 00 00    	je     80501ae <_vfiprintf_r+0xdf2>
				if (prec < 0)
					prec = -1;
				goto rflag;
			}
			n = 0;
			while (is_digit (ch)) {
 804f957:	8d 50 d0             	lea    -0x30(%eax),%edx
 804f95a:	31 ff                	xor    %edi,%edi
 804f95c:	83 fa 09             	cmp    $0x9,%edx
 804f95f:	0f 87 81 fb ff ff    	ja     804f4e6 <_vfiprintf_r+0x12a>
 804f965:	8d 76 00             	lea    0x0(%esi),%esi
				n = 10 * n + to_digit (ch);
 804f968:	8d 04 bf             	lea    (%edi,%edi,4),%eax
 804f96b:	8d 3c 42             	lea    (%edx,%eax,2),%edi
				ch = *fmt++;
 804f96e:	0f be 45 00          	movsbl 0x0(%ebp),%eax
 804f972:	45                   	inc    %ebp
				if (prec < 0)
					prec = -1;
				goto rflag;
			}
			n = 0;
			while (is_digit (ch)) {
 804f973:	8d 50 d0             	lea    -0x30(%eax),%edx
 804f976:	83 fa 09             	cmp    $0x9,%edx
 804f979:	76 ed                	jbe    804f968 <_vfiprintf_r+0x5ac>
 804f97b:	85 ff                	test   %edi,%edi
 804f97d:	0f 89 63 fb ff ff    	jns    804f4e6 <_vfiprintf_r+0x12a>
 804f983:	bf ff ff ff ff       	mov    $0xffffffff,%edi
 804f988:	e9 59 fb ff ff       	jmp    804f4e6 <_vfiprintf_r+0x12a>
		if (flags & HEXPREFIX)
			realsz+= 2;

		/* right-adjusting blank padding */
		if ((flags & (LADJUST|ZEROPAD)) == 0)
			PAD (width - realsz, blanks);
 804f98d:	8d 44 24 6c          	lea    0x6c(%esp),%eax
 804f991:	89 44 24 08          	mov    %eax,0x8(%esp)
 804f995:	8b 94 24 04 01 00 00 	mov    0x104(%esp),%edx
 804f99c:	89 54 24 04          	mov    %edx,0x4(%esp)
 804f9a0:	8b 84 24 00 01 00 00 	mov    0x100(%esp),%eax
 804f9a7:	89 04 24             	mov    %eax,(%esp)
 804f9aa:	e8 3d f9 ff ff       	call   804f2ec <__sprint_r>
 804f9af:	85 c0                	test   %eax,%eax
 804f9b1:	0f 85 c1 02 00 00    	jne    804fc78 <_vfiprintf_r+0x8bc>
 804f9b7:	8d 8c 24 a0 00 00 00 	lea    0xa0(%esp),%ecx
 804f9be:	66 90                	xchg   %ax,%ax
 804f9c0:	8b 44 24 70          	mov    0x70(%esp),%eax

		/* prefix */
		if (sign)
 804f9c4:	80 7c 24 69 00       	cmpb   $0x0,0x69(%esp)
 804f9c9:	74 22                	je     804f9ed <_vfiprintf_r+0x631>
			PRINT (&sign, 1);
 804f9cb:	8d 54 24 69          	lea    0x69(%esp),%edx
 804f9cf:	89 11                	mov    %edx,(%ecx)
 804f9d1:	c7 41 04 01 00 00 00 	movl   $0x1,0x4(%ecx)
 804f9d8:	ff 44 24 74          	incl   0x74(%esp)
 804f9dc:	40                   	inc    %eax
 804f9dd:	89 44 24 70          	mov    %eax,0x70(%esp)
 804f9e1:	83 f8 07             	cmp    $0x7,%eax
 804f9e4:	0f 8f 2a 05 00 00    	jg     804ff14 <_vfiprintf_r+0xb58>
 804f9ea:	83 c1 08             	add    $0x8,%ecx
		if (flags & HEXPREFIX)
 804f9ed:	8b 74 24 38          	mov    0x38(%esp),%esi
 804f9f1:	85 f6                	test   %esi,%esi
 804f9f3:	74 23                	je     804fa18 <_vfiprintf_r+0x65c>
			PRINT (ox, 2);
 804f9f5:	8d 54 24 6a          	lea    0x6a(%esp),%edx
 804f9f9:	89 11                	mov    %edx,(%ecx)
 804f9fb:	c7 41 04 02 00 00 00 	movl   $0x2,0x4(%ecx)
 804fa02:	83 44 24 74 02       	addl   $0x2,0x74(%esp)
 804fa07:	40                   	inc    %eax
 804fa08:	89 44 24 70          	mov    %eax,0x70(%esp)
 804fa0c:	83 f8 07             	cmp    $0x7,%eax
 804fa0f:	0f 8f 3b 05 00 00    	jg     804ff50 <_vfiprintf_r+0xb94>
 804fa15:	83 c1 08             	add    $0x8,%ecx

		/* right-adjusting zero padding */
		if ((flags & (LADJUST|ZEROPAD)) == ZEROPAD)
 804fa18:	81 7c 24 4c 80 00 00 	cmpl   $0x80,0x4c(%esp)
 804fa1f:	00 
 804fa20:	0f 84 76 02 00 00    	je     804fc9c <_vfiprintf_r+0x8e0>
			PAD (width - realsz, zeroes);

		/* leading zeroes from decimal precision */
		PAD (dprec - size, zeroes);
 804fa26:	8b 7c 24 40          	mov    0x40(%esp),%edi
 804fa2a:	2b 7c 24 3c          	sub    0x3c(%esp),%edi
 804fa2e:	85 ff                	test   %edi,%edi
 804fa30:	0f 8e 9a 00 00 00    	jle    804fad0 <_vfiprintf_r+0x714>
 804fa36:	83 ff 10             	cmp    $0x10,%edi
 804fa39:	7e 77                	jle    804fab2 <_vfiprintf_r+0x6f6>
 804fa3b:	8d 74 24 6c          	lea    0x6c(%esp),%esi
 804fa3f:	89 6c 24 40          	mov    %ebp,0x40(%esp)
 804fa43:	89 5c 24 38          	mov    %ebx,0x38(%esp)
 804fa47:	8b 9c 24 00 01 00 00 	mov    0x100(%esp),%ebx
 804fa4e:	8b ac 24 04 01 00 00 	mov    0x104(%esp),%ebp
 804fa55:	eb 0c                	jmp    804fa63 <_vfiprintf_r+0x6a7>
 804fa57:	90                   	nop
 804fa58:	83 c1 08             	add    $0x8,%ecx
 804fa5b:	83 ef 10             	sub    $0x10,%edi
 804fa5e:	83 ff 10             	cmp    $0x10,%edi
 804fa61:	7e 47                	jle    804faaa <_vfiprintf_r+0x6ee>
 804fa63:	c7 01 68 1b 05 08    	movl   $0x8051b68,(%ecx)
 804fa69:	c7 41 04 10 00 00 00 	movl   $0x10,0x4(%ecx)
 804fa70:	83 44 24 74 10       	addl   $0x10,0x74(%esp)
 804fa75:	40                   	inc    %eax
 804fa76:	89 44 24 70          	mov    %eax,0x70(%esp)
 804fa7a:	83 f8 07             	cmp    $0x7,%eax
 804fa7d:	7e d9                	jle    804fa58 <_vfiprintf_r+0x69c>
 804fa7f:	89 74 24 08          	mov    %esi,0x8(%esp)
 804fa83:	89 6c 24 04          	mov    %ebp,0x4(%esp)
 804fa87:	89 1c 24             	mov    %ebx,(%esp)
 804fa8a:	e8 5d f8 ff ff       	call   804f2ec <__sprint_r>
 804fa8f:	85 c0                	test   %eax,%eax
 804fa91:	0f 85 e1 01 00 00    	jne    804fc78 <_vfiprintf_r+0x8bc>
 804fa97:	8d 8c 24 a0 00 00 00 	lea    0xa0(%esp),%ecx
 804fa9e:	8b 44 24 70          	mov    0x70(%esp),%eax
 804faa2:	83 ef 10             	sub    $0x10,%edi
 804faa5:	83 ff 10             	cmp    $0x10,%edi
 804faa8:	7f b9                	jg     804fa63 <_vfiprintf_r+0x6a7>
 804faaa:	8b 6c 24 40          	mov    0x40(%esp),%ebp
 804faae:	8b 5c 24 38          	mov    0x38(%esp),%ebx
 804fab2:	c7 01 68 1b 05 08    	movl   $0x8051b68,(%ecx)
 804fab8:	89 79 04             	mov    %edi,0x4(%ecx)
 804fabb:	01 7c 24 74          	add    %edi,0x74(%esp)
 804fabf:	40                   	inc    %eax
 804fac0:	89 44 24 70          	mov    %eax,0x70(%esp)
 804fac4:	83 f8 07             	cmp    $0x7,%eax
 804fac7:	0f 8f 9f 03 00 00    	jg     804fe6c <_vfiprintf_r+0xab0>
 804facd:	83 c1 08             	add    $0x8,%ecx
					PRINT (cp, 1);
				PRINT (expstr, expsize);
			}
		}
#else /* !FLOATING_POINT */
		PRINT (cp, size);
 804fad0:	89 19                	mov    %ebx,(%ecx)
 804fad2:	8b 54 24 3c          	mov    0x3c(%esp),%edx
 804fad6:	89 51 04             	mov    %edx,0x4(%ecx)
 804fad9:	01 54 24 74          	add    %edx,0x74(%esp)
 804fadd:	40                   	inc    %eax
 804fade:	89 44 24 70          	mov    %eax,0x70(%esp)
 804fae2:	83 f8 07             	cmp    $0x7,%eax
 804fae5:	0f 8f 65 02 00 00    	jg     804fd50 <_vfiprintf_r+0x994>
 804faeb:	83 c1 08             	add    $0x8,%ecx
#endif
		/* left-adjusting padding (always blank) */
		if (flags & LADJUST)
 804faee:	f6 44 24 30 04       	testb  $0x4,0x30(%esp)
 804faf3:	0f 84 d3 00 00 00    	je     804fbcc <_vfiprintf_r+0x810>
			PAD (width - realsz, blanks);
 804faf9:	8b 74 24 48          	mov    0x48(%esp),%esi
 804fafd:	2b 74 24 34          	sub    0x34(%esp),%esi
 804fb01:	85 f6                	test   %esi,%esi
 804fb03:	0f 8e c3 00 00 00    	jle    804fbcc <_vfiprintf_r+0x810>
 804fb09:	8b 44 24 70          	mov    0x70(%esp),%eax
 804fb0d:	83 fe 10             	cmp    $0x10,%esi
 804fb10:	7e 72                	jle    804fb84 <_vfiprintf_r+0x7c8>
 804fb12:	8d 5c 24 6c          	lea    0x6c(%esp),%ebx
 804fb16:	89 6c 24 30          	mov    %ebp,0x30(%esp)
 804fb1a:	89 f5                	mov    %esi,%ebp
 804fb1c:	8b bc 24 00 01 00 00 	mov    0x100(%esp),%edi
 804fb23:	8b b4 24 04 01 00 00 	mov    0x104(%esp),%esi
 804fb2a:	eb 0b                	jmp    804fb37 <_vfiprintf_r+0x77b>
 804fb2c:	83 c1 08             	add    $0x8,%ecx
 804fb2f:	83 ed 10             	sub    $0x10,%ebp
 804fb32:	83 fd 10             	cmp    $0x10,%ebp
 804fb35:	7e 47                	jle    804fb7e <_vfiprintf_r+0x7c2>
 804fb37:	c7 01 58 1b 05 08    	movl   $0x8051b58,(%ecx)
 804fb3d:	c7 41 04 10 00 00 00 	movl   $0x10,0x4(%ecx)
 804fb44:	83 44 24 74 10       	addl   $0x10,0x74(%esp)
 804fb49:	40                   	inc    %eax
 804fb4a:	89 44 24 70          	mov    %eax,0x70(%esp)
 804fb4e:	83 f8 07             	cmp    $0x7,%eax
 804fb51:	7e d9                	jle    804fb2c <_vfiprintf_r+0x770>
 804fb53:	89 5c 24 08          	mov    %ebx,0x8(%esp)
 804fb57:	89 74 24 04          	mov    %esi,0x4(%esp)
 804fb5b:	89 3c 24             	mov    %edi,(%esp)
 804fb5e:	e8 89 f7 ff ff       	call   804f2ec <__sprint_r>
 804fb63:	85 c0                	test   %eax,%eax
 804fb65:	0f 85 0d 01 00 00    	jne    804fc78 <_vfiprintf_r+0x8bc>
 804fb6b:	8d 8c 24 a0 00 00 00 	lea    0xa0(%esp),%ecx
 804fb72:	8b 44 24 70          	mov    0x70(%esp),%eax
 804fb76:	83 ed 10             	sub    $0x10,%ebp
 804fb79:	83 fd 10             	cmp    $0x10,%ebp
 804fb7c:	7f b9                	jg     804fb37 <_vfiprintf_r+0x77b>
 804fb7e:	89 ee                	mov    %ebp,%esi
 804fb80:	8b 6c 24 30          	mov    0x30(%esp),%ebp
 804fb84:	c7 01 58 1b 05 08    	movl   $0x8051b58,(%ecx)
 804fb8a:	89 71 04             	mov    %esi,0x4(%ecx)
 804fb8d:	03 74 24 74          	add    0x74(%esp),%esi
 804fb91:	89 74 24 74          	mov    %esi,0x74(%esp)
 804fb95:	40                   	inc    %eax
 804fb96:	89 44 24 70          	mov    %eax,0x70(%esp)
 804fb9a:	83 f8 07             	cmp    $0x7,%eax
 804fb9d:	7e 31                	jle    804fbd0 <_vfiprintf_r+0x814>
 804fb9f:	8d 44 24 6c          	lea    0x6c(%esp),%eax
 804fba3:	89 44 24 08          	mov    %eax,0x8(%esp)
 804fba7:	8b 94 24 04 01 00 00 	mov    0x104(%esp),%edx
 804fbae:	89 54 24 04          	mov    %edx,0x4(%esp)
 804fbb2:	8b 84 24 00 01 00 00 	mov    0x100(%esp),%eax
 804fbb9:	89 04 24             	mov    %eax,(%esp)
 804fbbc:	e8 2b f7 ff ff       	call   804f2ec <__sprint_r>
 804fbc1:	85 c0                	test   %eax,%eax
 804fbc3:	0f 85 af 00 00 00    	jne    804fc78 <_vfiprintf_r+0x8bc>
 804fbc9:	8d 76 00             	lea    0x0(%esi),%esi
 804fbcc:	8b 74 24 74          	mov    0x74(%esp),%esi

		/* finally, adjust ret */
		ret += width > realsz ? width : realsz;
 804fbd0:	8b 44 24 34          	mov    0x34(%esp),%eax
 804fbd4:	3b 44 24 48          	cmp    0x48(%esp),%eax
 804fbd8:	7d 04                	jge    804fbde <_vfiprintf_r+0x822>
 804fbda:	8b 44 24 48          	mov    0x48(%esp),%eax
 804fbde:	01 44 24 44          	add    %eax,0x44(%esp)

		FLUSH ();	/* copy out the I/O vectors */
 804fbe2:	85 f6                	test   %esi,%esi
 804fbe4:	0f 85 9e 01 00 00    	jne    804fd88 <_vfiprintf_r+0x9cc>
 804fbea:	c7 44 24 70 00 00 00 	movl   $0x0,0x70(%esp)
 804fbf1:	00 
 804fbf2:	8d 8c 24 a0 00 00 00 	lea    0xa0(%esp),%ecx
 804fbf9:	e9 76 f8 ff ff       	jmp    804f474 <_vfiprintf_r+0xb8>
 804fbfe:	66 90                	xchg   %ax,%ax
				/*
				 * Unsigned mod is hard, and unsigned mod
				 * by a constant is easier than that by
				 * a variable; hence this switch.
				 */
				switch (base) {
 804fc00:	3c 01                	cmp    $0x1,%al
 804fc02:	0f 84 c4 02 00 00    	je     804fecc <_vfiprintf_r+0xb10>
			/*
			 * ``The result of converting a zero value with an
			 * explicit precision of zero is no characters.''
			 *	-- ANSI X3J11
			 */
			cp = buf + BUF;
 804fc08:	8d 9c 24 a0 00 00 00 	lea    0xa0(%esp),%ebx
				/*
				 * Unsigned mod is hard, and unsigned mod
				 * by a constant is easier than that by
				 * a variable; hence this switch.
				 */
				switch (base) {
 804fc0f:	3c 02                	cmp    $0x2,%al
 804fc11:	0f 84 91 02 00 00    	je     804fea8 <_vfiprintf_r+0xaec>
				case OCT:
					do {
						*--cp = to_char (_uquad & 7);
 804fc17:	4b                   	dec    %ebx
 804fc18:	89 f0                	mov    %esi,%eax
 804fc1a:	83 e0 07             	and    $0x7,%eax
 804fc1d:	83 c0 30             	add    $0x30,%eax
 804fc20:	88 03                	mov    %al,(%ebx)
						_uquad >>= 3;
					} while (_uquad);
 804fc22:	c1 ee 03             	shr    $0x3,%esi
 804fc25:	75 f0                	jne    804fc17 <_vfiprintf_r+0x85b>
				 * a variable; hence this switch.
				 */
				switch (base) {
				case OCT:
					do {
						*--cp = to_char (_uquad & 7);
 804fc27:	89 da                	mov    %ebx,%edx
						_uquad >>= 3;
					} while (_uquad);
					/* handle octal leading 0 */
					if (flags & ALT && *cp != '0')
 804fc29:	f6 44 24 30 01       	testb  $0x1,0x30(%esp)
 804fc2e:	74 0d                	je     804fc3d <_vfiprintf_r+0x881>
 804fc30:	3c 30                	cmp    $0x30,%al
 804fc32:	0f 84 cb 02 00 00    	je     804ff03 <_vfiprintf_r+0xb47>
						*--cp = '0';
 804fc38:	4b                   	dec    %ebx
 804fc39:	c6 42 ff 30          	movb   $0x30,-0x1(%edx)
 804fc3d:	8b 44 24 2c          	mov    0x2c(%esp),%eax
 804fc41:	29 d8                	sub    %ebx,%eax
 804fc43:	89 44 24 3c          	mov    %eax,0x3c(%esp)
 804fc47:	e9 34 fa ff ff       	jmp    804f680 <_vfiprintf_r+0x2c4>
			_free_r (data, malloc_buf);
			malloc_buf = NULL;
		}
	}
done:
	FLUSH ();
 804fc4c:	8b 54 24 74          	mov    0x74(%esp),%edx
 804fc50:	85 d2                	test   %edx,%edx
 804fc52:	74 24                	je     804fc78 <_vfiprintf_r+0x8bc>
 804fc54:	8d 44 24 6c          	lea    0x6c(%esp),%eax
 804fc58:	89 44 24 08          	mov    %eax,0x8(%esp)
 804fc5c:	8b 94 24 04 01 00 00 	mov    0x104(%esp),%edx
 804fc63:	89 54 24 04          	mov    %edx,0x4(%esp)
 804fc67:	8b 84 24 00 01 00 00 	mov    0x100(%esp),%eax
 804fc6e:	89 04 24             	mov    %eax,(%esp)
 804fc71:	e8 76 f6 ff ff       	call   804f2ec <__sprint_r>
 804fc76:	66 90                	xchg   %ax,%ax
	if (malloc_buf != NULL)
		_free_r (data, malloc_buf);
#ifndef STRING_ONLY
	_newlib_flockfile_end (fp);
#endif
	return (__sferror (fp) ? EOF : ret);
 804fc78:	8b 94 24 04 01 00 00 	mov    0x104(%esp),%edx
 804fc7f:	f6 42 0c 40          	testb  $0x40,0xc(%edx)
 804fc83:	74 08                	je     804fc8d <_vfiprintf_r+0x8d1>
	ORIENT(fp, -1);

	/* sorry, fprintf(read_only_file, "") returns EOF, not 0 */
	if (cantwrite (data, fp)) {
		_newlib_flockfile_exit (fp);
		return (EOF);
 804fc85:	c7 44 24 44 ff ff ff 	movl   $0xffffffff,0x44(%esp)
 804fc8c:	ff 
#ifndef STRING_ONLY
	_newlib_flockfile_end (fp);
#endif
	return (__sferror (fp) ? EOF : ret);
	/* NOTREACHED */
}
 804fc8d:	8b 44 24 44          	mov    0x44(%esp),%eax
 804fc91:	81 c4 ec 00 00 00    	add    $0xec,%esp
 804fc97:	5b                   	pop    %ebx
 804fc98:	5e                   	pop    %esi
 804fc99:	5f                   	pop    %edi
 804fc9a:	5d                   	pop    %ebp
 804fc9b:	c3                   	ret    
		if (flags & HEXPREFIX)
			PRINT (ox, 2);

		/* right-adjusting zero padding */
		if ((flags & (LADJUST|ZEROPAD)) == ZEROPAD)
			PAD (width - realsz, zeroes);
 804fc9c:	8b 7c 24 48          	mov    0x48(%esp),%edi
 804fca0:	2b 7c 24 34          	sub    0x34(%esp),%edi
 804fca4:	85 ff                	test   %edi,%edi
 804fca6:	0f 8e 7a fd ff ff    	jle    804fa26 <_vfiprintf_r+0x66a>
 804fcac:	83 ff 10             	cmp    $0x10,%edi
 804fcaf:	7e 79                	jle    804fd2a <_vfiprintf_r+0x96e>
 804fcb1:	8d 74 24 6c          	lea    0x6c(%esp),%esi
 804fcb5:	89 6c 24 38          	mov    %ebp,0x38(%esp)
 804fcb9:	89 5c 24 4c          	mov    %ebx,0x4c(%esp)
 804fcbd:	8b 9c 24 00 01 00 00 	mov    0x100(%esp),%ebx
 804fcc4:	8b ac 24 04 01 00 00 	mov    0x104(%esp),%ebp
 804fccb:	eb 0e                	jmp    804fcdb <_vfiprintf_r+0x91f>
 804fccd:	8d 76 00             	lea    0x0(%esi),%esi
 804fcd0:	83 c1 08             	add    $0x8,%ecx
 804fcd3:	83 ef 10             	sub    $0x10,%edi
 804fcd6:	83 ff 10             	cmp    $0x10,%edi
 804fcd9:	7e 47                	jle    804fd22 <_vfiprintf_r+0x966>
 804fcdb:	c7 01 68 1b 05 08    	movl   $0x8051b68,(%ecx)
 804fce1:	c7 41 04 10 00 00 00 	movl   $0x10,0x4(%ecx)
 804fce8:	83 44 24 74 10       	addl   $0x10,0x74(%esp)
 804fced:	40                   	inc    %eax
 804fcee:	89 44 24 70          	mov    %eax,0x70(%esp)
 804fcf2:	83 f8 07             	cmp    $0x7,%eax
 804fcf5:	7e d9                	jle    804fcd0 <_vfiprintf_r+0x914>
 804fcf7:	89 74 24 08          	mov    %esi,0x8(%esp)
 804fcfb:	89 6c 24 04          	mov    %ebp,0x4(%esp)
 804fcff:	89 1c 24             	mov    %ebx,(%esp)
 804fd02:	e8 e5 f5 ff ff       	call   804f2ec <__sprint_r>
 804fd07:	85 c0                	test   %eax,%eax
 804fd09:	0f 85 69 ff ff ff    	jne    804fc78 <_vfiprintf_r+0x8bc>
 804fd0f:	8d 8c 24 a0 00 00 00 	lea    0xa0(%esp),%ecx
 804fd16:	8b 44 24 70          	mov    0x70(%esp),%eax
 804fd1a:	83 ef 10             	sub    $0x10,%edi
 804fd1d:	83 ff 10             	cmp    $0x10,%edi
 804fd20:	7f b9                	jg     804fcdb <_vfiprintf_r+0x91f>
 804fd22:	8b 6c 24 38          	mov    0x38(%esp),%ebp
 804fd26:	8b 5c 24 4c          	mov    0x4c(%esp),%ebx
 804fd2a:	c7 01 68 1b 05 08    	movl   $0x8051b68,(%ecx)
 804fd30:	89 79 04             	mov    %edi,0x4(%ecx)
 804fd33:	01 7c 24 74          	add    %edi,0x74(%esp)
 804fd37:	40                   	inc    %eax
 804fd38:	89 44 24 70          	mov    %eax,0x70(%esp)
 804fd3c:	83 f8 07             	cmp    $0x7,%eax
 804fd3f:	0f 8f 8b 03 00 00    	jg     80500d0 <_vfiprintf_r+0xd14>
 804fd45:	83 c1 08             	add    $0x8,%ecx
 804fd48:	e9 d9 fc ff ff       	jmp    804fa26 <_vfiprintf_r+0x66a>
 804fd4d:	8d 76 00             	lea    0x0(%esi),%esi
					PRINT (cp, 1);
				PRINT (expstr, expsize);
			}
		}
#else /* !FLOATING_POINT */
		PRINT (cp, size);
 804fd50:	8d 44 24 6c          	lea    0x6c(%esp),%eax
 804fd54:	89 44 24 08          	mov    %eax,0x8(%esp)
 804fd58:	8b 94 24 04 01 00 00 	mov    0x104(%esp),%edx
 804fd5f:	89 54 24 04          	mov    %edx,0x4(%esp)
 804fd63:	8b 84 24 00 01 00 00 	mov    0x100(%esp),%eax
 804fd6a:	89 04 24             	mov    %eax,(%esp)
 804fd6d:	e8 7a f5 ff ff       	call   804f2ec <__sprint_r>
 804fd72:	85 c0                	test   %eax,%eax
 804fd74:	0f 85 fe fe ff ff    	jne    804fc78 <_vfiprintf_r+0x8bc>
 804fd7a:	8d 8c 24 a0 00 00 00 	lea    0xa0(%esp),%ecx
 804fd81:	e9 68 fd ff ff       	jmp    804faee <_vfiprintf_r+0x732>
 804fd86:	66 90                	xchg   %ax,%ax
			PAD (width - realsz, blanks);

		/* finally, adjust ret */
		ret += width > realsz ? width : realsz;

		FLUSH ();	/* copy out the I/O vectors */
 804fd88:	8d 44 24 6c          	lea    0x6c(%esp),%eax
 804fd8c:	89 44 24 08          	mov    %eax,0x8(%esp)
 804fd90:	8b 94 24 04 01 00 00 	mov    0x104(%esp),%edx
 804fd97:	89 54 24 04          	mov    %edx,0x4(%esp)
 804fd9b:	8b 84 24 00 01 00 00 	mov    0x100(%esp),%eax
 804fda2:	89 04 24             	mov    %eax,(%esp)
 804fda5:	e8 42 f5 ff ff       	call   804f2ec <__sprint_r>
 804fdaa:	85 c0                	test   %eax,%eax
 804fdac:	0f 84 38 fe ff ff    	je     804fbea <_vfiprintf_r+0x82e>
 804fdb2:	e9 c1 fe ff ff       	jmp    804fc78 <_vfiprintf_r+0x8bc>
 804fdb7:	90                   	nop
	_newlib_flockfile_start (fp);

	ORIENT(fp, -1);

	/* sorry, fprintf(read_only_file, "") returns EOF, not 0 */
	if (cantwrite (data, fp)) {
 804fdb8:	8b 94 24 04 01 00 00 	mov    0x104(%esp),%edx
 804fdbf:	89 54 24 04          	mov    %edx,0x4(%esp)
 804fdc3:	8b 84 24 00 01 00 00 	mov    0x100(%esp),%eax
 804fdca:	89 04 24             	mov    %eax,(%esp)
 804fdcd:	e8 e6 af ff ff       	call   804adb8 <__swsetup_r>
 804fdd2:	85 c0                	test   %eax,%eax
 804fdd4:	0f 85 ab fe ff ff    	jne    804fc85 <_vfiprintf_r+0x8c9>
 804fdda:	8b 94 24 04 01 00 00 	mov    0x104(%esp),%edx
 804fde1:	8b 42 0c             	mov    0xc(%edx),%eax
		return (EOF);
	}

#ifdef _UNBUF_STREAM_OPT
	/* optimise fprintf(stderr) (and other unbuffered Unix files) */
	if ((fp->_flags & (__SNBF|__SWR|__SRW)) == (__SNBF|__SWR) &&
 804fde4:	83 e0 1a             	and    $0x1a,%eax
 804fde7:	66 83 f8 0a          	cmp    $0xa,%ax
 804fdeb:	0f 85 36 f6 ff ff    	jne    804f427 <_vfiprintf_r+0x6b>
 804fdf1:	8b 94 24 04 01 00 00 	mov    0x104(%esp),%edx
 804fdf8:	66 83 7a 0e 00       	cmpw   $0x0,0xe(%edx)
 804fdfd:	0f 88 24 f6 ff ff    	js     804f427 <_vfiprintf_r+0x6b>
	    fp->_file >= 0) {
		_newlib_flockfile_exit (fp);
		return (__sbprintf (data, fp, fmt0, ap));
 804fe03:	8b 8c 24 0c 01 00 00 	mov    0x10c(%esp),%ecx
 804fe0a:	89 0c 24             	mov    %ecx,(%esp)
 804fe0d:	8b 8c 24 08 01 00 00 	mov    0x108(%esp),%ecx
 804fe14:	8b 84 24 00 01 00 00 	mov    0x100(%esp),%eax
 804fe1b:	e8 bc 03 00 00       	call   80501dc <__sbprintf>
 804fe20:	89 44 24 44          	mov    %eax,0x44(%esp)
#ifndef STRING_ONLY
	_newlib_flockfile_end (fp);
#endif
	return (__sferror (fp) ? EOF : ret);
	/* NOTREACHED */
}
 804fe24:	8b 44 24 44          	mov    0x44(%esp),%eax
 804fe28:	81 c4 ec 00 00 00    	add    $0xec,%esp
 804fe2e:	5b                   	pop    %ebx
 804fe2f:	5e                   	pop    %esi
 804fe30:	5f                   	pop    %edi
 804fe31:	5d                   	pop    %ebp
 804fe32:	c3                   	ret    
 804fe33:	90                   	nop
#else
                while (*fmt != '\0' && *fmt != '%')
                    fmt += 1;
#endif
		if ((m = fmt - cp) != 0) {
			PRINT (cp, m);
 804fe34:	8d 44 24 6c          	lea    0x6c(%esp),%eax
 804fe38:	89 44 24 08          	mov    %eax,0x8(%esp)
 804fe3c:	8b 94 24 04 01 00 00 	mov    0x104(%esp),%edx
 804fe43:	89 54 24 04          	mov    %edx,0x4(%esp)
 804fe47:	8b 84 24 00 01 00 00 	mov    0x100(%esp),%eax
 804fe4e:	89 04 24             	mov    %eax,(%esp)
 804fe51:	e8 96 f4 ff ff       	call   804f2ec <__sprint_r>
 804fe56:	85 c0                	test   %eax,%eax
 804fe58:	0f 85 1a fe ff ff    	jne    804fc78 <_vfiprintf_r+0x8bc>
 804fe5e:	8d 8c 24 a0 00 00 00 	lea    0xa0(%esp),%ecx
 804fe65:	e9 4b f6 ff ff       	jmp    804f4b5 <_vfiprintf_r+0xf9>
 804fe6a:	66 90                	xchg   %ax,%ax
		/* right-adjusting zero padding */
		if ((flags & (LADJUST|ZEROPAD)) == ZEROPAD)
			PAD (width - realsz, zeroes);

		/* leading zeroes from decimal precision */
		PAD (dprec - size, zeroes);
 804fe6c:	8d 44 24 6c          	lea    0x6c(%esp),%eax
 804fe70:	89 44 24 08          	mov    %eax,0x8(%esp)
 804fe74:	8b 94 24 04 01 00 00 	mov    0x104(%esp),%edx
 804fe7b:	89 54 24 04          	mov    %edx,0x4(%esp)
 804fe7f:	8b 84 24 00 01 00 00 	mov    0x100(%esp),%eax
 804fe86:	89 04 24             	mov    %eax,(%esp)
 804fe89:	e8 5e f4 ff ff       	call   804f2ec <__sprint_r>
 804fe8e:	85 c0                	test   %eax,%eax
 804fe90:	0f 85 e2 fd ff ff    	jne    804fc78 <_vfiprintf_r+0x8bc>
 804fe96:	8d 8c 24 a0 00 00 00 	lea    0xa0(%esp),%ecx
 804fe9d:	8b 44 24 70          	mov    0x70(%esp),%eax
 804fea1:	e9 2a fc ff ff       	jmp    804fad0 <_vfiprintf_r+0x714>
 804fea6:	66 90                	xchg   %ax,%ax
			/*
			 * ``The result of converting a zero value with an
			 * explicit precision of zero is no characters.''
			 *	-- ANSI X3J11
			 */
			cp = buf + BUF;
 804fea8:	8b 54 24 50          	mov    0x50(%esp),%edx
					} while (_uquad != 0);
					break;

				case HEX:
					do {
						*--cp = xdigs[_uquad & 15];
 804feac:	4b                   	dec    %ebx
 804fead:	89 f0                	mov    %esi,%eax
 804feaf:	83 e0 0f             	and    $0xf,%eax
 804feb2:	8a 04 02             	mov    (%edx,%eax,1),%al
 804feb5:	88 03                	mov    %al,(%ebx)
						_uquad >>= 4;
					} while (_uquad);
 804feb7:	c1 ee 04             	shr    $0x4,%esi
 804feba:	75 f0                	jne    804feac <_vfiprintf_r+0xaf0>
 804febc:	8b 44 24 2c          	mov    0x2c(%esp),%eax
 804fec0:	29 d8                	sub    %ebx,%eax
 804fec2:	89 44 24 3c          	mov    %eax,0x3c(%esp)
 804fec6:	e9 b5 f7 ff ff       	jmp    804f680 <_vfiprintf_r+0x2c4>
 804fecb:	90                   	nop
						*--cp = '0';
					break;

				case DEC:
					/* many numbers are 1 digit */
					if (_uquad < 10) {
 804fecc:	83 fe 09             	cmp    $0x9,%esi
 804fecf:	0f 86 20 01 00 00    	jbe    804fff5 <_vfiprintf_r+0xc39>
			/*
			 * ``The result of converting a zero value with an
			 * explicit precision of zero is no characters.''
			 *	-- ANSI X3J11
			 */
			cp = buf + BUF;
 804fed5:	8d 9c 24 a0 00 00 00 	lea    0xa0(%esp),%ebx
					       next cases. */
					    if (grouping[1] != '\0')
					      grouping++;
					  }
#endif
					  _uquad /= 10;
 804fedc:	89 4c 24 34          	mov    %ecx,0x34(%esp)
					}
#ifdef _WANT_IO_C99_FORMATS
					ndig = 0;
#endif
					do {
					  *--cp = to_char (_uquad % 10);
 804fee0:	4b                   	dec    %ebx
 804fee1:	89 f0                	mov    %esi,%eax
 804fee3:	b9 0a 00 00 00       	mov    $0xa,%ecx
 804fee8:	31 d2                	xor    %edx,%edx
 804feea:	f7 f1                	div    %ecx
 804feec:	83 c2 30             	add    $0x30,%edx
 804feef:	88 13                	mov    %dl,(%ebx)
					       next cases. */
					    if (grouping[1] != '\0')
					      grouping++;
					  }
#endif
					  _uquad /= 10;
 804fef1:	b8 cd cc cc cc       	mov    $0xcccccccd,%eax
 804fef6:	f7 e6                	mul    %esi
					} while (_uquad != 0);
 804fef8:	89 d6                	mov    %edx,%esi
 804fefa:	c1 ee 03             	shr    $0x3,%esi
 804fefd:	75 e1                	jne    804fee0 <_vfiprintf_r+0xb24>
 804feff:	8b 4c 24 34          	mov    0x34(%esp),%ecx
 804ff03:	8b 54 24 2c          	mov    0x2c(%esp),%edx
 804ff07:	29 da                	sub    %ebx,%edx
 804ff09:	89 54 24 3c          	mov    %edx,0x3c(%esp)
 804ff0d:	e9 6e f7 ff ff       	jmp    804f680 <_vfiprintf_r+0x2c4>
 804ff12:	66 90                	xchg   %ax,%ax
		if ((flags & (LADJUST|ZEROPAD)) == 0)
			PAD (width - realsz, blanks);

		/* prefix */
		if (sign)
			PRINT (&sign, 1);
 804ff14:	8d 44 24 6c          	lea    0x6c(%esp),%eax
 804ff18:	89 44 24 08          	mov    %eax,0x8(%esp)
 804ff1c:	8b 94 24 04 01 00 00 	mov    0x104(%esp),%edx
 804ff23:	89 54 24 04          	mov    %edx,0x4(%esp)
 804ff27:	8b 84 24 00 01 00 00 	mov    0x100(%esp),%eax
 804ff2e:	89 04 24             	mov    %eax,(%esp)
 804ff31:	e8 b6 f3 ff ff       	call   804f2ec <__sprint_r>
 804ff36:	85 c0                	test   %eax,%eax
 804ff38:	0f 85 3a fd ff ff    	jne    804fc78 <_vfiprintf_r+0x8bc>
 804ff3e:	8d 8c 24 a0 00 00 00 	lea    0xa0(%esp),%ecx
 804ff45:	8b 44 24 70          	mov    0x70(%esp),%eax
 804ff49:	e9 9f fa ff ff       	jmp    804f9ed <_vfiprintf_r+0x631>
 804ff4e:	66 90                	xchg   %ax,%ax
		if (flags & HEXPREFIX)
			PRINT (ox, 2);
 804ff50:	8d 44 24 6c          	lea    0x6c(%esp),%eax
 804ff54:	89 44 24 08          	mov    %eax,0x8(%esp)
 804ff58:	8b 94 24 04 01 00 00 	mov    0x104(%esp),%edx
 804ff5f:	89 54 24 04          	mov    %edx,0x4(%esp)
 804ff63:	8b 84 24 00 01 00 00 	mov    0x100(%esp),%eax
 804ff6a:	89 04 24             	mov    %eax,(%esp)
 804ff6d:	e8 7a f3 ff ff       	call   804f2ec <__sprint_r>
 804ff72:	85 c0                	test   %eax,%eax
 804ff74:	0f 85 fe fc ff ff    	jne    804fc78 <_vfiprintf_r+0x8bc>
 804ff7a:	8d 8c 24 a0 00 00 00 	lea    0xa0(%esp),%ecx
 804ff81:	8b 44 24 70          	mov    0x70(%esp),%eax
 804ff85:	e9 8e fa ff ff       	jmp    804fa18 <_vfiprintf_r+0x65c>
	    (u_long)GET_ARG (N, ap, u_int))
#endif

#ifndef STRING_ONLY
	/* Initialize std streams if not dealing with sprintf family.  */
	CHECK_INIT (data, fp);
 804ff8a:	89 04 24             	mov    %eax,(%esp)
 804ff8d:	e8 d2 c8 ff ff       	call   804c864 <__sinit>
 804ff92:	e9 4c f4 ff ff       	jmp    804f3e3 <_vfiprintf_r+0x27>
#ifndef STRING_ONLY
	_newlib_flockfile_end (fp);
#endif
	return (__sferror (fp) ? EOF : ret);
	/* NOTREACHED */
}
 804ff97:	8b 84 24 0c 01 00 00 	mov    0x10c(%esp),%eax
			continue;	/* no output */
		case 'O': /* extension */
			flags |= LONGINT;
			/*FALLTHROUGH*/
		case 'o':
			_uquad = UARG ();
 804ff9e:	f6 44 24 30 40       	testb  $0x40,0x30(%esp)
 804ffa3:	0f 84 e5 00 00 00    	je     805008e <_vfiprintf_r+0xcd2>
#ifndef STRING_ONLY
	_newlib_flockfile_end (fp);
#endif
	return (__sferror (fp) ? EOF : ret);
	/* NOTREACHED */
}
 804ffa9:	8b 00                	mov    (%eax),%eax
 804ffab:	0f b7 f0             	movzwl %ax,%esi
			continue;	/* no output */
		case 'O': /* extension */
			flags |= LONGINT;
			/*FALLTHROUGH*/
		case 'o':
			_uquad = UARG ();
 804ffae:	8b 94 24 0c 01 00 00 	mov    0x10c(%esp),%edx
 804ffb5:	83 c2 04             	add    $0x4,%edx
 804ffb8:	89 94 24 0c 01 00 00 	mov    %edx,0x10c(%esp)
			base = OCT;
 804ffbf:	31 c0                	xor    %eax,%eax
 804ffc1:	e9 76 f6 ff ff       	jmp    804f63c <_vfiprintf_r+0x280>
#ifndef STRING_ONLY
	_newlib_flockfile_end (fp);
#endif
	return (__sferror (fp) ? EOF : ret);
	/* NOTREACHED */
}
 804ffc6:	8b 94 24 0c 01 00 00 	mov    0x10c(%esp),%edx
			break;
		case 'U': /* extension */
			flags |= LONGINT;
			/*FALLTHROUGH*/
		case 'u':
			_uquad = UARG ();
 804ffcd:	f6 44 24 30 40       	testb  $0x40,0x30(%esp)
 804ffd2:	0f 84 88 00 00 00    	je     8050060 <_vfiprintf_r+0xca4>
#ifndef STRING_ONLY
	_newlib_flockfile_end (fp);
#endif
	return (__sferror (fp) ? EOF : ret);
	/* NOTREACHED */
}
 804ffd8:	8b 12                	mov    (%edx),%edx
 804ffda:	0f b7 f2             	movzwl %dx,%esi
			break;
		case 'U': /* extension */
			flags |= LONGINT;
			/*FALLTHROUGH*/
		case 'u':
			_uquad = UARG ();
 804ffdd:	8b 84 24 0c 01 00 00 	mov    0x10c(%esp),%eax
 804ffe4:	83 c0 04             	add    $0x4,%eax
 804ffe7:	89 84 24 0c 01 00 00 	mov    %eax,0x10c(%esp)
			base = DEC;
 804ffee:	b0 01                	mov    $0x1,%al
 804fff0:	e9 47 f6 ff ff       	jmp    804f63c <_vfiprintf_r+0x280>
					break;

				case DEC:
					/* many numbers are 1 digit */
					if (_uquad < 10) {
						*--cp = to_char(_uquad);
 804fff5:	8d 46 30             	lea    0x30(%esi),%eax
 804fff8:	88 84 24 9f 00 00 00 	mov    %al,0x9f(%esp)
 804ffff:	8b 54 24 5c          	mov    0x5c(%esp),%edx
 8050003:	89 54 24 3c          	mov    %edx,0x3c(%esp)
 8050007:	8d 9c 24 9f 00 00 00 	lea    0x9f(%esp),%ebx
						break;
 805000e:	e9 6d f6 ff ff       	jmp    804f680 <_vfiprintf_r+0x2c4>
						fmt = old_fmt;
						goto rflag;
					}
				}
#endif /* !_NO_POS_ARGS */
				prec = GET_ARG (n, ap, int);
 8050013:	89 84 24 0c 01 00 00 	mov    %eax,0x10c(%esp)
 805001a:	e9 c2 f4 ff ff       	jmp    804f4e1 <_vfiprintf_r+0x125>
			*
			* To demonstrate this case, compile and run:
                        *    printf ("%#.0o",0);
			*/
                       else if (base == OCT && (flags & ALT))
                         *--cp = '0';
 805001f:	c6 84 24 9f 00 00 00 	movb   $0x30,0x9f(%esp)
 8050026:	30 
 8050027:	8b 54 24 5c          	mov    0x5c(%esp),%edx
 805002b:	89 54 24 3c          	mov    %edx,0x3c(%esp)
 805002f:	8d 9c 24 9f 00 00 00 	lea    0x9f(%esp),%ebx
 8050036:	e9 45 f6 ff ff       	jmp    804f680 <_vfiprintf_r+0x2c4>
#ifdef _WANT_IO_C99_FORMATS
			else if (flags & CHARINT)
				*GET_ARG (N, ap, char_ptr_t) = ret;
#endif
			else
				*GET_ARG (N, ap, int_ptr_t) = ret;
 805003b:	8b 94 24 0c 01 00 00 	mov    0x10c(%esp),%edx
 8050042:	8b 02                	mov    (%edx),%eax
 8050044:	8b 54 24 44          	mov    0x44(%esp),%edx
 8050048:	89 10                	mov    %edx,(%eax)
 805004a:	8b 84 24 0c 01 00 00 	mov    0x10c(%esp),%eax
 8050051:	83 c0 04             	add    $0x4,%eax
 8050054:	89 84 24 0c 01 00 00 	mov    %eax,0x10c(%esp)
 805005b:	e9 14 f4 ff ff       	jmp    804f474 <_vfiprintf_r+0xb8>
			break;
		case 'U': /* extension */
			flags |= LONGINT;
			/*FALLTHROUGH*/
		case 'u':
			_uquad = UARG ();
 8050060:	8b 32                	mov    (%edx),%esi
 8050062:	83 c2 04             	add    $0x4,%edx
 8050065:	89 94 24 0c 01 00 00 	mov    %edx,0x10c(%esp)
			base = DEC;
 805006c:	b0 01                	mov    $0x1,%al
 805006e:	e9 c9 f5 ff ff       	jmp    804f63c <_vfiprintf_r+0x280>
#ifndef STRING_ONLY
	_newlib_flockfile_end (fp);
#endif
	return (__sferror (fp) ? EOF : ret);
	/* NOTREACHED */
}
 8050073:	8b 12                	mov    (%edx),%edx
 8050075:	0f b7 f2             	movzwl %dx,%esi
		case 'X':
			xdigs = "0123456789ABCDEF";
			goto hex;
		case 'x':
			xdigs = "0123456789abcdef";
hex:			_uquad = UARG ();
 8050078:	8b 94 24 0c 01 00 00 	mov    0x10c(%esp),%edx
 805007f:	83 c2 04             	add    $0x4,%edx
 8050082:	89 94 24 0c 01 00 00 	mov    %edx,0x10c(%esp)
 8050089:	e9 ee f6 ff ff       	jmp    804f77c <_vfiprintf_r+0x3c0>
			continue;	/* no output */
		case 'O': /* extension */
			flags |= LONGINT;
			/*FALLTHROUGH*/
		case 'o':
			_uquad = UARG ();
 805008e:	8b 30                	mov    (%eax),%esi
 8050090:	83 c0 04             	add    $0x4,%eax
 8050093:	89 84 24 0c 01 00 00 	mov    %eax,0x10c(%esp)
			base = OCT;
 805009a:	31 c0                	xor    %eax,%eax
 805009c:	e9 9b f5 ff ff       	jmp    804f63c <_vfiprintf_r+0x280>
		case 'D':  /* extension */
			flags |= LONGINT;
			/*FALLTHROUGH*/
		case 'd':
		case 'i':
			_uquad = SARG ();
 80500a1:	8b 84 24 0c 01 00 00 	mov    0x10c(%esp),%eax
 80500a8:	0f bf 30             	movswl (%eax),%esi
 80500ab:	83 c0 04             	add    $0x4,%eax
 80500ae:	89 84 24 0c 01 00 00 	mov    %eax,0x10c(%esp)
#ifndef _NO_LONGLONG
			if ((quad_t)_uquad < 0)
#else
			if ((long) _uquad < 0)
 80500b5:	85 f6                	test   %esi,%esi
 80500b7:	0f 89 23 f7 ff ff    	jns    804f7e0 <_vfiprintf_r+0x424>
#endif
			{

				_uquad = -_uquad;
 80500bd:	f7 de                	neg    %esi
				sign = '-';
 80500bf:	c6 44 24 69 2d       	movb   $0x2d,0x69(%esp)
 80500c4:	c6 44 24 38 2d       	movb   $0x2d,0x38(%esp)
			}
			base = DEC;
 80500c9:	b0 01                	mov    $0x1,%al
 80500cb:	e9 76 f5 ff ff       	jmp    804f646 <_vfiprintf_r+0x28a>
		if (flags & HEXPREFIX)
			PRINT (ox, 2);

		/* right-adjusting zero padding */
		if ((flags & (LADJUST|ZEROPAD)) == ZEROPAD)
			PAD (width - realsz, zeroes);
 80500d0:	8d 44 24 6c          	lea    0x6c(%esp),%eax
 80500d4:	89 44 24 08          	mov    %eax,0x8(%esp)
 80500d8:	8b 94 24 04 01 00 00 	mov    0x104(%esp),%edx
 80500df:	89 54 24 04          	mov    %edx,0x4(%esp)
 80500e3:	8b 84 24 00 01 00 00 	mov    0x100(%esp),%eax
 80500ea:	89 04 24             	mov    %eax,(%esp)
 80500ed:	e8 fa f1 ff ff       	call   804f2ec <__sprint_r>
 80500f2:	85 c0                	test   %eax,%eax
 80500f4:	0f 85 7e fb ff ff    	jne    804fc78 <_vfiprintf_r+0x8bc>
 80500fa:	8d 8c 24 a0 00 00 00 	lea    0xa0(%esp),%ecx
 8050101:	8b 44 24 70          	mov    0x70(%esp),%eax
 8050105:	e9 1c f9 ff ff       	jmp    804fa26 <_vfiprintf_r+0x66a>
 805010a:	8a 44 24 69          	mov    0x69(%esp),%al
 805010e:	88 44 24 38          	mov    %al,0x38(%esp)
			goto nosign;
		case 's':
#ifdef _WANT_IO_C99_FORMATS
		case 'S':
#endif
			cp = GET_ARG (N, ap, char_ptr_t);
 8050112:	89 b4 24 0c 01 00 00 	mov    %esi,0x10c(%esp)
#endif
		fmt_anchor = fmt;
		fmt++;		/* skip over '%' */

		flags = 0;
		dprec = 0;
 8050119:	c7 44 24 40 00 00 00 	movl   $0x0,0x40(%esp)
 8050120:	00 
 8050121:	e9 5e f5 ff ff       	jmp    804f684 <_vfiprintf_r+0x2c8>
			   NULL string when precision is not 0.
			   However, if we are not optimizing for size,
			   we might as well mirror glibc behavior.  */
			if (cp == NULL) {
				cp = "(null)";
				size = ((unsigned) prec > 6U) ? 6 : prec;
 8050126:	89 7c 24 3c          	mov    %edi,0x3c(%esp)
 805012a:	83 ff 06             	cmp    $0x6,%edi
 805012d:	76 08                	jbe    8050137 <_vfiprintf_r+0xd7b>
 805012f:	c7 44 24 3c 06 00 00 	movl   $0x6,0x3c(%esp)
 8050136:	00 
 8050137:	8b 44 24 3c          	mov    0x3c(%esp),%eax
 805013b:	89 44 24 34          	mov    %eax,0x34(%esp)
 805013f:	85 c0                	test   %eax,%eax
 8050141:	78 61                	js     80501a4 <_vfiprintf_r+0xde8>
			goto nosign;
		case 's':
#ifdef _WANT_IO_C99_FORMATS
		case 'S':
#endif
			cp = GET_ARG (N, ap, char_ptr_t);
 8050143:	89 b4 24 0c 01 00 00 	mov    %esi,0x10c(%esp)
			/* Behavior is undefined if the user passed a
			   NULL string when precision is not 0.
			   However, if we are not optimizing for size,
			   we might as well mirror glibc behavior.  */
			if (cp == NULL) {
				cp = "(null)";
 805014a:	bb c6 16 05 08       	mov    $0x80516c6,%ebx
 805014f:	e9 c9 f3 ff ff       	jmp    804f51d <_vfiprintf_r+0x161>
					if (size > prec)
						size = prec;
				} else
					size = prec;
			} else
				size = strlen (cp);
 8050154:	89 1c 24             	mov    %ebx,(%esp)
 8050157:	89 4c 24 18          	mov    %ecx,0x18(%esp)
 805015b:	e8 90 8a ff ff       	call   8048bf0 <strlen>
 8050160:	89 44 24 3c          	mov    %eax,0x3c(%esp)
 8050164:	8a 54 24 69          	mov    0x69(%esp),%dl
 8050168:	88 54 24 38          	mov    %dl,0x38(%esp)
			goto nosign;
		case 's':
#ifdef _WANT_IO_C99_FORMATS
		case 'S':
#endif
			cp = GET_ARG (N, ap, char_ptr_t);
 805016c:	89 b4 24 0c 01 00 00 	mov    %esi,0x10c(%esp)
#endif
		fmt_anchor = fmt;
		fmt++;		/* skip over '%' */

		flags = 0;
		dprec = 0;
 8050173:	c7 44 24 40 00 00 00 	movl   $0x0,0x40(%esp)
 805017a:	00 
 805017b:	8b 4c 24 18          	mov    0x18(%esp),%ecx
 805017f:	e9 00 f5 ff ff       	jmp    804f684 <_vfiprintf_r+0x2c8>
 8050184:	8a 44 24 69          	mov    0x69(%esp),%al
 8050188:	88 44 24 38          	mov    %al,0x38(%esp)
				 * NUL in the first `prec' characters, and
				 * strlen () will go further.
				 */
				char *p = memchr (cp, 0, prec);

				if (p != NULL) {
 805018c:	89 7c 24 3c          	mov    %edi,0x3c(%esp)
			goto nosign;
		case 's':
#ifdef _WANT_IO_C99_FORMATS
		case 'S':
#endif
			cp = GET_ARG (N, ap, char_ptr_t);
 8050190:	89 b4 24 0c 01 00 00 	mov    %esi,0x10c(%esp)
#endif
		fmt_anchor = fmt;
		fmt++;		/* skip over '%' */

		flags = 0;
		dprec = 0;
 8050197:	c7 44 24 40 00 00 00 	movl   $0x0,0x40(%esp)
 805019e:	00 
 805019f:	e9 e0 f4 ff ff       	jmp    804f684 <_vfiprintf_r+0x2c8>
 80501a4:	c7 44 24 34 00 00 00 	movl   $0x0,0x34(%esp)
 80501ab:	00 
 80501ac:	eb 95                	jmp    8050143 <_vfiprintf_r+0xd87>
						fmt = old_fmt;
						goto rflag;
					}
				}
#endif /* !_NO_POS_ARGS */
				prec = GET_ARG (n, ap, int);
 80501ae:	8b 84 24 0c 01 00 00 	mov    0x10c(%esp),%eax
 80501b5:	83 c0 04             	add    $0x4,%eax
 80501b8:	8b 94 24 0c 01 00 00 	mov    0x10c(%esp),%edx
 80501bf:	8b 3a                	mov    (%edx),%edi
#ifndef _NO_POS_ARGS
				is_pos_arg = old_is_pos_arg;
#endif
				if (prec < 0)
 80501c1:	85 ff                	test   %edi,%edi
 80501c3:	0f 89 4a fe ff ff    	jns    8050013 <_vfiprintf_r+0xc57>
						fmt = old_fmt;
						goto rflag;
					}
				}
#endif /* !_NO_POS_ARGS */
				prec = GET_ARG (n, ap, int);
 80501c9:	89 84 24 0c 01 00 00 	mov    %eax,0x10c(%esp)
#ifndef _NO_POS_ARGS
				is_pos_arg = old_is_pos_arg;
#endif
				if (prec < 0)
					prec = -1;
 80501d0:	bf ff ff ff ff       	mov    $0xffffffff,%edi
 80501d5:	e9 07 f3 ff ff       	jmp    804f4e1 <_vfiprintf_r+0x125>
 80501da:	66 90                	xchg   %ax,%ax

080501dc <__sbprintf>:
_DEFUN(__sbprintf, (rptr, fp, fmt, ap),
       struct _reent *rptr _AND
       register FILE *fp   _AND
       _CONST char *fmt  _AND
       va_list ap)
{
 80501dc:	55                   	push   %ebp
 80501dd:	57                   	push   %edi
 80501de:	56                   	push   %esi
 80501df:	53                   	push   %ebx
 80501e0:	81 ec 8c 04 00 00    	sub    $0x48c,%esp
 80501e6:	89 c7                	mov    %eax,%edi
 80501e8:	89 d3                	mov    %edx,%ebx
	int ret;
	FILE fake;
	unsigned char buf[BUFSIZ];

	/* copy the important variables */
	fake._flags = fp->_flags & ~__SNBF;
 80501ea:	8b 52 0c             	mov    0xc(%edx),%edx
 80501ed:	83 e2 fd             	and    $0xfffffffd,%edx
 80501f0:	66 89 54 24 24       	mov    %dx,0x24(%esp)
	fake._flags2 = fp->_flags2;
 80501f5:	8b 43 64             	mov    0x64(%ebx),%eax
 80501f8:	89 44 24 7c          	mov    %eax,0x7c(%esp)
	fake._file = fp->_file;
 80501fc:	66 8b 43 0e          	mov    0xe(%ebx),%ax
 8050200:	66 89 44 24 26       	mov    %ax,0x26(%esp)
	fake._cookie = fp->_cookie;
 8050205:	8b 43 1c             	mov    0x1c(%ebx),%eax
 8050208:	89 44 24 34          	mov    %eax,0x34(%esp)
	fake._write = fp->_write;
 805020c:	8b 43 24             	mov    0x24(%ebx),%eax
 805020f:	89 44 24 3c          	mov    %eax,0x3c(%esp)

	/* set up the buffer */
	fake._bf._base = fake._p = buf;
 8050213:	8d 84 24 80 00 00 00 	lea    0x80(%esp),%eax
 805021a:	89 44 24 18          	mov    %eax,0x18(%esp)
 805021e:	89 44 24 28          	mov    %eax,0x28(%esp)
	fake._bf._size = fake._w = sizeof (buf);
 8050222:	c7 44 24 20 00 04 00 	movl   $0x400,0x20(%esp)
 8050229:	00 
 805022a:	c7 44 24 2c 00 04 00 	movl   $0x400,0x2c(%esp)
 8050231:	00 
	fake._lbfsize = 0;	/* not actually used, but Just In Case */
 8050232:	c7 44 24 30 00 00 00 	movl   $0x0,0x30(%esp)
 8050239:	00 
#ifndef __SINGLE_THREAD__
	__lock_init_recursive (fake._lock);
#endif

	/* do the work, then copy any error status */
	ret = _VFPRINTF_R (rptr, &fake, fmt, ap);
 805023a:	8b 84 24 a0 04 00 00 	mov    0x4a0(%esp),%eax
 8050241:	89 44 24 0c          	mov    %eax,0xc(%esp)
 8050245:	89 4c 24 08          	mov    %ecx,0x8(%esp)
 8050249:	8d 6c 24 18          	lea    0x18(%esp),%ebp
 805024d:	89 6c 24 04          	mov    %ebp,0x4(%esp)
 8050251:	89 3c 24             	mov    %edi,(%esp)
 8050254:	e8 63 f1 ff ff       	call   804f3bc <_vfiprintf_r>
 8050259:	89 c6                	mov    %eax,%esi
	if (ret >= 0 && _fflush_r (rptr, &fake))
 805025b:	85 c0                	test   %eax,%eax
 805025d:	78 10                	js     805026f <__sbprintf+0x93>
 805025f:	89 6c 24 04          	mov    %ebp,0x4(%esp)
 8050263:	89 3c 24             	mov    %edi,(%esp)
 8050266:	e8 55 c4 ff ff       	call   804c6c0 <_fflush_r>
 805026b:	85 c0                	test   %eax,%eax
 805026d:	75 19                	jne    8050288 <__sbprintf+0xac>
		ret = EOF;
	if (fake._flags & __SERR)
 805026f:	f6 44 24 24 40       	testb  $0x40,0x24(%esp)
 8050274:	74 05                	je     805027b <__sbprintf+0x9f>
		fp->_flags |= __SERR;
 8050276:	66 83 4b 0c 40       	orw    $0x40,0xc(%ebx)

#ifndef __SINGLE_THREAD__
	__lock_close_recursive (fake._lock);
#endif
	return (ret);
}
 805027b:	89 f0                	mov    %esi,%eax
 805027d:	81 c4 8c 04 00 00    	add    $0x48c,%esp
 8050283:	5b                   	pop    %ebx
 8050284:	5e                   	pop    %esi
 8050285:	5f                   	pop    %edi
 8050286:	5d                   	pop    %ebp
 8050287:	c3                   	ret    
#endif

	/* do the work, then copy any error status */
	ret = _VFPRINTF_R (rptr, &fake, fmt, ap);
	if (ret >= 0 && _fflush_r (rptr, &fake))
		ret = EOF;
 8050288:	be ff ff ff ff       	mov    $0xffffffff,%esi
 805028d:	eb e0                	jmp    805026f <__sbprintf+0x93>
 805028f:	90                   	nop

08050290 <vfiprintf>:
int
_DEFUN(VFPRINTF, (fp, fmt0, ap),
       FILE * fp         _AND
       _CONST char *fmt0 _AND
       va_list ap)
{
 8050290:	83 ec 1c             	sub    $0x1c,%esp
  int result;
  result = _VFPRINTF_R (_REENT, fp, fmt0, ap);
 8050293:	8b 44 24 28          	mov    0x28(%esp),%eax
 8050297:	89 44 24 0c          	mov    %eax,0xc(%esp)
 805029b:	8b 44 24 24          	mov    0x24(%esp),%eax
 805029f:	89 44 24 08          	mov    %eax,0x8(%esp)
 80502a3:	8b 44 24 20          	mov    0x20(%esp),%eax
 80502a7:	89 44 24 04          	mov    %eax,0x4(%esp)
 80502ab:	a1 40 40 05 08       	mov    0x8054040,%eax
 80502b0:	89 04 24             	mov    %eax,(%esp)
 80502b3:	e8 04 f1 ff ff       	call   804f3bc <_vfiprintf_r>
  return result;
}
 80502b8:	83 c4 1c             	add    $0x1c,%esp
 80502bb:	c3                   	ret    

080502bc <_write_r>:
_DEFUN (_write_r, (ptr, fd, buf, cnt),
     struct _reent *ptr _AND
     int fd _AND
     _CONST _PTR buf _AND
     size_t cnt)
{
 80502bc:	83 ec 1c             	sub    $0x1c,%esp
  _ssize_t ret;

  errno = 0;
 80502bf:	c7 05 f4 49 05 08 00 	movl   $0x0,0x80549f4
 80502c6:	00 00 00 
  if ((ret = (_ssize_t)_write (fd, buf, cnt)) == -1 && errno != 0)
 80502c9:	8b 44 24 2c          	mov    0x2c(%esp),%eax
 80502cd:	89 44 24 08          	mov    %eax,0x8(%esp)
 80502d1:	8b 44 24 28          	mov    0x28(%esp),%eax
 80502d5:	89 44 24 04          	mov    %eax,0x4(%esp)
 80502d9:	8b 44 24 24          	mov    0x24(%esp),%eax
 80502dd:	89 04 24             	mov    %eax,(%esp)
 80502e0:	e8 3f 10 00 00       	call   8051324 <write>
 80502e5:	83 f8 ff             	cmp    $0xffffffff,%eax
 80502e8:	74 06                	je     80502f0 <_write_r+0x34>
    ptr->_errno = errno;
  return ret;
}
 80502ea:	83 c4 1c             	add    $0x1c,%esp
 80502ed:	c3                   	ret    
 80502ee:	66 90                	xchg   %ax,%ax
     size_t cnt)
{
  _ssize_t ret;

  errno = 0;
  if ((ret = (_ssize_t)_write (fd, buf, cnt)) == -1 && errno != 0)
 80502f0:	8b 15 f4 49 05 08    	mov    0x80549f4,%edx
 80502f6:	85 d2                	test   %edx,%edx
 80502f8:	74 f0                	je     80502ea <_write_r+0x2e>
    ptr->_errno = errno;
 80502fa:	8b 4c 24 20          	mov    0x20(%esp),%ecx
 80502fe:	89 11                	mov    %edx,(%ecx)
  return ret;
}
 8050300:	83 c4 1c             	add    $0x1c,%esp
 8050303:	c3                   	ret    

08050304 <_calloc_r>:
#if __STD_C
Void_t* cALLOc(RARG size_t n, size_t elem_size)
#else
Void_t* cALLOc(RARG n, elem_size) RDECL size_t n; size_t elem_size;
#endif
{
 8050304:	53                   	push   %ebx
 8050305:	83 ec 18             	sub    $0x18,%esp
 8050308:	8b 44 24 28          	mov    0x28(%esp),%eax
  mchunkptr p;
  INTERNAL_SIZE_T csz;

  INTERNAL_SIZE_T sz = n * elem_size;
 805030c:	0f af 44 24 24       	imul   0x24(%esp),%eax
  MALLOC_LOCK;
  oldtop = top;
  oldtopsize = chunksize(top);
#endif

  mem = mALLOc (RCALL sz);
 8050311:	89 44 24 04          	mov    %eax,0x4(%esp)
 8050315:	8b 44 24 20          	mov    0x20(%esp),%eax
 8050319:	89 04 24             	mov    %eax,(%esp)
 805031c:	e8 9b d1 ff ff       	call   804d4bc <_malloc_r>
 8050321:	89 c3                	mov    %eax,%ebx

  if (mem == 0) 
 8050323:	85 c0                	test   %eax,%eax
 8050325:	74 29                	je     8050350 <_calloc_r+0x4c>
#endif
      return mem;
    }
#endif

    csz = chunksize(p);
 8050327:	8b 40 fc             	mov    -0x4(%eax),%eax
 805032a:	83 e0 fc             	and    $0xfffffffc,%eax
      csz = oldtopsize;
    }
    MALLOC_UNLOCK;
#endif

    MALLOC_ZERO(mem, csz - SIZE_SZ);
 805032d:	83 e8 04             	sub    $0x4,%eax
 8050330:	83 f8 24             	cmp    $0x24,%eax
 8050333:	77 4f                	ja     8050384 <_calloc_r+0x80>
 8050335:	83 f8 13             	cmp    $0x13,%eax
 8050338:	77 1e                	ja     8050358 <_calloc_r+0x54>
 805033a:	89 d8                	mov    %ebx,%eax
 805033c:	c7 00 00 00 00 00    	movl   $0x0,(%eax)
 8050342:	c7 40 04 00 00 00 00 	movl   $0x0,0x4(%eax)
 8050349:	c7 40 08 00 00 00 00 	movl   $0x0,0x8(%eax)
    return mem;
  }
}
 8050350:	89 d8                	mov    %ebx,%eax
 8050352:	83 c4 18             	add    $0x18,%esp
 8050355:	5b                   	pop    %ebx
 8050356:	c3                   	ret    
 8050357:	90                   	nop
      csz = oldtopsize;
    }
    MALLOC_UNLOCK;
#endif

    MALLOC_ZERO(mem, csz - SIZE_SZ);
 8050358:	c7 03 00 00 00 00    	movl   $0x0,(%ebx)
 805035e:	c7 43 04 00 00 00 00 	movl   $0x0,0x4(%ebx)
 8050365:	83 f8 1b             	cmp    $0x1b,%eax
 8050368:	76 36                	jbe    80503a0 <_calloc_r+0x9c>
 805036a:	c7 43 08 00 00 00 00 	movl   $0x0,0x8(%ebx)
 8050371:	c7 43 0c 00 00 00 00 	movl   $0x0,0xc(%ebx)
 8050378:	83 f8 24             	cmp    $0x24,%eax
 805037b:	74 2b                	je     80503a8 <_calloc_r+0xa4>
 805037d:	8d 43 10             	lea    0x10(%ebx),%eax
 8050380:	eb ba                	jmp    805033c <_calloc_r+0x38>
 8050382:	66 90                	xchg   %ax,%ax
 8050384:	89 44 24 08          	mov    %eax,0x8(%esp)
 8050388:	c7 44 24 04 00 00 00 	movl   $0x0,0x4(%esp)
 805038f:	00 
 8050390:	89 1c 24             	mov    %ebx,(%esp)
 8050393:	e8 88 d9 ff ff       	call   804dd20 <memset>
    return mem;
  }
}
 8050398:	89 d8                	mov    %ebx,%eax
 805039a:	83 c4 18             	add    $0x18,%esp
 805039d:	5b                   	pop    %ebx
 805039e:	c3                   	ret    
 805039f:	90                   	nop
      csz = oldtopsize;
    }
    MALLOC_UNLOCK;
#endif

    MALLOC_ZERO(mem, csz - SIZE_SZ);
 80503a0:	8d 43 08             	lea    0x8(%ebx),%eax
 80503a3:	eb 97                	jmp    805033c <_calloc_r+0x38>
 80503a5:	8d 76 00             	lea    0x0(%esi),%esi
 80503a8:	c7 43 10 00 00 00 00 	movl   $0x0,0x10(%ebx)
 80503af:	c7 43 14 00 00 00 00 	movl   $0x0,0x14(%ebx)
 80503b6:	8d 43 18             	lea    0x18(%ebx),%eax
 80503b9:	eb 81                	jmp    805033c <_calloc_r+0x38>
 80503bb:	90                   	nop

080503bc <_close_r>:

int
_close_r (ptr, fd)
     struct _reent *ptr;
     int fd;
{
 80503bc:	83 ec 1c             	sub    $0x1c,%esp
  int ret;

  errno = 0;
 80503bf:	c7 05 f4 49 05 08 00 	movl   $0x0,0x80549f4
 80503c6:	00 00 00 
  if ((ret = _close (fd)) == -1 && errno != 0)
 80503c9:	8b 44 24 24          	mov    0x24(%esp),%eax
 80503cd:	89 04 24             	mov    %eax,(%esp)
 80503d0:	e8 8f 07 00 00       	call   8050b64 <close>
 80503d5:	83 f8 ff             	cmp    $0xffffffff,%eax
 80503d8:	74 06                	je     80503e0 <_close_r+0x24>
    ptr->_errno = errno;
  return ret;
}
 80503da:	83 c4 1c             	add    $0x1c,%esp
 80503dd:	c3                   	ret    
 80503de:	66 90                	xchg   %ax,%ax
     int fd;
{
  int ret;

  errno = 0;
  if ((ret = _close (fd)) == -1 && errno != 0)
 80503e0:	8b 15 f4 49 05 08    	mov    0x80549f4,%edx
 80503e6:	85 d2                	test   %edx,%edx
 80503e8:	74 f0                	je     80503da <_close_r+0x1e>
    ptr->_errno = errno;
 80503ea:	8b 4c 24 20          	mov    0x20(%esp),%ecx
 80503ee:	89 11                	mov    %edx,(%ecx)
  return ret;
}
 80503f0:	83 c4 1c             	add    $0x1c,%esp
 80503f3:	c3                   	ret    

080503f4 <_fclose_r>:

int
_DEFUN(_fclose_r, (rptr, fp),
      struct _reent *rptr _AND
      register FILE * fp)
{
 80503f4:	57                   	push   %edi
 80503f5:	56                   	push   %esi
 80503f6:	53                   	push   %ebx
 80503f7:	83 ec 10             	sub    $0x10,%esp
 80503fa:	8b 74 24 20          	mov    0x20(%esp),%esi
 80503fe:	8b 5c 24 24          	mov    0x24(%esp),%ebx
  int r;

  if (fp == NULL)
 8050402:	85 db                	test   %ebx,%ebx
 8050404:	74 16                	je     805041c <_fclose_r+0x28>
    return (0);			/* on NULL */

  CHECK_INIT (rptr, fp);
 8050406:	85 f6                	test   %esi,%esi
 8050408:	74 0b                	je     8050415 <_fclose_r+0x21>
 805040a:	8b 46 38             	mov    0x38(%esi),%eax
 805040d:	85 c0                	test   %eax,%eax
 805040f:	0f 84 93 00 00 00    	je     80504a8 <_fclose_r+0xb4>
  int __oldcancel;
  pthread_setcancelstate (PTHREAD_CANCEL_DISABLE, &__oldcancel);
#endif
  _flockfile (fp);

  if (fp->_flags == 0)		/* not open! */
 8050415:	66 83 7b 0c 00       	cmpw   $0x0,0xc(%ebx)
 805041a:	75 0c                	jne    8050428 <_fclose_r+0x34>
      register FILE * fp)
{
  int r;

  if (fp == NULL)
    return (0);			/* on NULL */
 805041c:	31 ff                	xor    %edi,%edi
#ifdef _STDIO_WITH_THREAD_CANCELLATION_SUPPORT
  pthread_setcancelstate (__oldcancel, &__oldcancel);
#endif

  return (r);
}
 805041e:	89 f8                	mov    %edi,%eax
 8050420:	83 c4 10             	add    $0x10,%esp
 8050423:	5b                   	pop    %ebx
 8050424:	5e                   	pop    %esi
 8050425:	5f                   	pop    %edi
 8050426:	c3                   	ret    
 8050427:	90                   	nop
      return (0);
    }
  /* Unconditionally flush to allow special handling for seekable read
     files to reposition file to last byte processed as opposed to
     last byte read ahead into the buffer.  */
  r = _fflush_r (rptr, fp);
 8050428:	89 5c 24 04          	mov    %ebx,0x4(%esp)
 805042c:	89 34 24             	mov    %esi,(%esp)
 805042f:	e8 8c c2 ff ff       	call   804c6c0 <_fflush_r>
 8050434:	89 c7                	mov    %eax,%edi
  if (fp->_close != NULL && fp->_close (rptr, fp->_cookie) < 0)
 8050436:	8b 43 2c             	mov    0x2c(%ebx),%eax
 8050439:	85 c0                	test   %eax,%eax
 805043b:	74 10                	je     805044d <_fclose_r+0x59>
 805043d:	8b 53 1c             	mov    0x1c(%ebx),%edx
 8050440:	89 54 24 04          	mov    %edx,0x4(%esp)
 8050444:	89 34 24             	mov    %esi,(%esp)
 8050447:	ff d0                	call   *%eax
 8050449:	85 c0                	test   %eax,%eax
 805044b:	78 6b                	js     80504b8 <_fclose_r+0xc4>
    r = EOF;
  if (fp->_flags & __SMBF)
 805044d:	f6 43 0c 80          	testb  $0x80,0xc(%ebx)
 8050451:	75 6d                	jne    80504c0 <_fclose_r+0xcc>
    _free_r (rptr, (char *) fp->_bf._base);
  if (HASUB (fp))
 8050453:	8b 43 30             	mov    0x30(%ebx),%eax
 8050456:	85 c0                	test   %eax,%eax
 8050458:	74 1a                	je     8050474 <_fclose_r+0x80>
    FREEUB (rptr, fp);
 805045a:	8d 53 40             	lea    0x40(%ebx),%edx
 805045d:	39 d0                	cmp    %edx,%eax
 805045f:	74 0c                	je     805046d <_fclose_r+0x79>
 8050461:	89 44 24 04          	mov    %eax,0x4(%esp)
 8050465:	89 34 24             	mov    %esi,(%esp)
 8050468:	e8 93 c6 ff ff       	call   804cb00 <_free_r>
 805046d:	c7 43 30 00 00 00 00 	movl   $0x0,0x30(%ebx)
  if (HASLB (fp))
 8050474:	8b 43 44             	mov    0x44(%ebx),%eax
 8050477:	85 c0                	test   %eax,%eax
 8050479:	74 13                	je     805048e <_fclose_r+0x9a>
    FREELB (rptr, fp);
 805047b:	89 44 24 04          	mov    %eax,0x4(%esp)
 805047f:	89 34 24             	mov    %esi,(%esp)
 8050482:	e8 79 c6 ff ff       	call   804cb00 <_free_r>
 8050487:	c7 43 44 00 00 00 00 	movl   $0x0,0x44(%ebx)
  __sfp_lock_acquire ();
 805048e:	e8 4d c5 ff ff       	call   804c9e0 <__sfp_lock_acquire>
  fp->_flags = 0;		/* release this FILE for reuse */
 8050493:	66 c7 43 0c 00 00    	movw   $0x0,0xc(%ebx)
  _funlockfile (fp);
#ifndef __SINGLE_THREAD__
  __lock_close_recursive (fp->_lock);
#endif

  __sfp_lock_release ();
 8050499:	e8 46 c5 ff ff       	call   804c9e4 <__sfp_lock_release>
#ifdef _STDIO_WITH_THREAD_CANCELLATION_SUPPORT
  pthread_setcancelstate (__oldcancel, &__oldcancel);
#endif

  return (r);
}
 805049e:	89 f8                	mov    %edi,%eax
 80504a0:	83 c4 10             	add    $0x10,%esp
 80504a3:	5b                   	pop    %ebx
 80504a4:	5e                   	pop    %esi
 80504a5:	5f                   	pop    %edi
 80504a6:	c3                   	ret    
 80504a7:	90                   	nop
  int r;

  if (fp == NULL)
    return (0);			/* on NULL */

  CHECK_INIT (rptr, fp);
 80504a8:	89 34 24             	mov    %esi,(%esp)
 80504ab:	e8 b4 c3 ff ff       	call   804c864 <__sinit>
 80504b0:	e9 60 ff ff ff       	jmp    8050415 <_fclose_r+0x21>
 80504b5:	8d 76 00             	lea    0x0(%esi),%esi
  /* Unconditionally flush to allow special handling for seekable read
     files to reposition file to last byte processed as opposed to
     last byte read ahead into the buffer.  */
  r = _fflush_r (rptr, fp);
  if (fp->_close != NULL && fp->_close (rptr, fp->_cookie) < 0)
    r = EOF;
 80504b8:	bf ff ff ff ff       	mov    $0xffffffff,%edi
 80504bd:	eb 8e                	jmp    805044d <_fclose_r+0x59>
 80504bf:	90                   	nop
  if (fp->_flags & __SMBF)
    _free_r (rptr, (char *) fp->_bf._base);
 80504c0:	8b 43 10             	mov    0x10(%ebx),%eax
 80504c3:	89 44 24 04          	mov    %eax,0x4(%esp)
 80504c7:	89 34 24             	mov    %esi,(%esp)
 80504ca:	e8 31 c6 ff ff       	call   804cb00 <_free_r>
 80504cf:	eb 82                	jmp    8050453 <_fclose_r+0x5f>
 80504d1:	8d 76 00             	lea    0x0(%esi),%esi

080504d4 <fclose>:
#ifndef _REENT_ONLY

int
_DEFUN(fclose, (fp),
       register FILE * fp)
{
 80504d4:	83 ec 1c             	sub    $0x1c,%esp
  return _fclose_r(_REENT, fp);
 80504d7:	8b 44 24 20          	mov    0x20(%esp),%eax
 80504db:	89 44 24 04          	mov    %eax,0x4(%esp)
 80504df:	a1 40 40 05 08       	mov    0x8054040,%eax
 80504e4:	89 04 24             	mov    %eax,(%esp)
 80504e7:	e8 08 ff ff ff       	call   80503f4 <_fclose_r>
}
 80504ec:	83 c4 1c             	add    $0x1c,%esp
 80504ef:	c3                   	ret    

080504f0 <_fputwc_r>:
wint_t
_DEFUN(_fputwc_r, (ptr, wc, fp),
	struct _reent *ptr _AND
	wchar_t wc _AND
	FILE *fp)
{
 80504f0:	55                   	push   %ebp
 80504f1:	57                   	push   %edi
 80504f2:	56                   	push   %esi
 80504f3:	53                   	push   %ebx
 80504f4:	83 ec 2c             	sub    $0x2c,%esp
 80504f7:	8b 5c 24 48          	mov    0x48(%esp),%ebx
  wint_t r;

  _newlib_flockfile_start (fp);
  ORIENT(fp, 1);
 80504fb:	8b 43 0c             	mov    0xc(%ebx),%eax
 80504fe:	f6 c4 20             	test   $0x20,%ah
 8050501:	75 0e                	jne    8050511 <_fputwc_r+0x21>
 8050503:	80 cc 20             	or     $0x20,%ah
 8050506:	66 89 43 0c          	mov    %ax,0xc(%ebx)
 805050a:	81 4b 64 00 20 00 00 	orl    $0x2000,0x64(%ebx)
	FILE *fp)
{
  char buf[MB_LEN_MAX];
  size_t i, len;

  if (MB_CUR_MAX == 1 && wc > 0 && wc <= UCHAR_MAX)
 8050511:	e8 02 ce ff ff       	call   804d318 <__locale_mb_cur_max>
 8050516:	48                   	dec    %eax
 8050517:	0f 84 bb 00 00 00    	je     80505d8 <_fputwc_r+0xe8>
      *buf = (unsigned char)wc;
      len = 1;
    }
  else
    {
      if ((len = _wcrtomb_r (ptr, buf, wc, &fp->_mbstate)) == (size_t) -1)
 805051d:	8d 43 5c             	lea    0x5c(%ebx),%eax
 8050520:	89 44 24 0c          	mov    %eax,0xc(%esp)
 8050524:	8b 54 24 44          	mov    0x44(%esp),%edx
 8050528:	89 54 24 08          	mov    %edx,0x8(%esp)
 805052c:	8d 6c 24 1f          	lea    0x1f(%esp),%ebp
 8050530:	89 6c 24 04          	mov    %ebp,0x4(%esp)
 8050534:	8b 54 24 40          	mov    0x40(%esp),%edx
 8050538:	89 14 24             	mov    %edx,(%esp)
 805053b:	e8 58 0f 00 00       	call   8051498 <_wcrtomb_r>
 8050540:	89 c7                	mov    %eax,%edi
 8050542:	83 f8 ff             	cmp    $0xffffffff,%eax
 8050545:	74 51                	je     8050598 <_fputwc_r+0xa8>
	  fp->_flags |= __SERR;
	  return WEOF;
	}
    }

  for (i = 0; i < len; i++)
 8050547:	85 c0                	test   %eax,%eax
 8050549:	74 61                	je     80505ac <_fputwc_r+0xbc>
       * Assume single-byte locale with no special encoding.
       * A more careful test would be to check
       * _CurrentRuneLocale->encoding.
       */
      *buf = (unsigned char)wc;
      len = 1;
 805054b:	31 f6                	xor    %esi,%esi
 805054d:	eb 10                	jmp    805055f <_fputwc_r+0x6f>
 805054f:	90                   	nop
	  return WEOF;
	}
    }

  for (i = 0; i < len; i++)
    if (__sputc_r (ptr, (unsigned char) buf[i], fp) == EOF)
 8050550:	8b 03                	mov    (%ebx),%eax
 8050552:	8a 54 35 00          	mov    0x0(%ebp,%esi,1),%dl
 8050556:	88 10                	mov    %dl,(%eax)
 8050558:	ff 03                	incl   (%ebx)
	  fp->_flags |= __SERR;
	  return WEOF;
	}
    }

  for (i = 0; i < len; i++)
 805055a:	46                   	inc    %esi
 805055b:	39 fe                	cmp    %edi,%esi
 805055d:	73 4d                	jae    80505ac <_fputwc_r+0xbc>
    if (__sputc_r (ptr, (unsigned char) buf[i], fp) == EOF)
 805055f:	8b 43 08             	mov    0x8(%ebx),%eax
 8050562:	48                   	dec    %eax
 8050563:	89 43 08             	mov    %eax,0x8(%ebx)
 8050566:	85 c0                	test   %eax,%eax
 8050568:	79 e6                	jns    8050550 <_fputwc_r+0x60>
 805056a:	3b 43 18             	cmp    0x18(%ebx),%eax
 805056d:	7c 49                	jl     80505b8 <_fputwc_r+0xc8>
 805056f:	8b 03                	mov    (%ebx),%eax
 8050571:	8a 54 35 00          	mov    0x0(%ebp,%esi,1),%dl
 8050575:	88 10                	mov    %dl,(%eax)
 8050577:	8b 03                	mov    (%ebx),%eax
 8050579:	80 38 0a             	cmpb   $0xa,(%eax)
 805057c:	0f 84 86 00 00 00    	je     8050608 <_fputwc_r+0x118>
 8050582:	31 d2                	xor    %edx,%edx
 8050584:	40                   	inc    %eax
 8050585:	89 03                	mov    %eax,(%ebx)
 8050587:	84 d2                	test   %dl,%dl
 8050589:	74 cf                	je     805055a <_fputwc_r+0x6a>
      return WEOF;
 805058b:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
  _newlib_flockfile_start (fp);
  ORIENT(fp, 1);
  r = __fputwc(ptr, wc, fp);
  _newlib_flockfile_end (fp);
  return r;
}
 8050590:	83 c4 2c             	add    $0x2c,%esp
 8050593:	5b                   	pop    %ebx
 8050594:	5e                   	pop    %esi
 8050595:	5f                   	pop    %edi
 8050596:	5d                   	pop    %ebp
 8050597:	c3                   	ret    
    }
  else
    {
      if ((len = _wcrtomb_r (ptr, buf, wc, &fp->_mbstate)) == (size_t) -1)
	{
	  fp->_flags |= __SERR;
 8050598:	66 83 4b 0c 40       	orw    $0x40,0xc(%ebx)
	  return WEOF;
 805059d:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
  _newlib_flockfile_start (fp);
  ORIENT(fp, 1);
  r = __fputwc(ptr, wc, fp);
  _newlib_flockfile_end (fp);
  return r;
}
 80505a2:	83 c4 2c             	add    $0x2c,%esp
 80505a5:	5b                   	pop    %ebx
 80505a6:	5e                   	pop    %esi
 80505a7:	5f                   	pop    %edi
 80505a8:	5d                   	pop    %ebp
 80505a9:	c3                   	ret    
 80505aa:	66 90                	xchg   %ax,%ax

  for (i = 0; i < len; i++)
    if (__sputc_r (ptr, (unsigned char) buf[i], fp) == EOF)
      return WEOF;

  return (wint_t) wc;
 80505ac:	8b 44 24 44          	mov    0x44(%esp),%eax
  _newlib_flockfile_start (fp);
  ORIENT(fp, 1);
  r = __fputwc(ptr, wc, fp);
  _newlib_flockfile_end (fp);
  return r;
}
 80505b0:	83 c4 2c             	add    $0x2c,%esp
 80505b3:	5b                   	pop    %ebx
 80505b4:	5e                   	pop    %esi
 80505b5:	5f                   	pop    %edi
 80505b6:	5d                   	pop    %ebp
 80505b7:	c3                   	ret    
	  return WEOF;
	}
    }

  for (i = 0; i < len; i++)
    if (__sputc_r (ptr, (unsigned char) buf[i], fp) == EOF)
 80505b8:	89 5c 24 08          	mov    %ebx,0x8(%esp)
 80505bc:	0f b6 44 35 00       	movzbl 0x0(%ebp,%esi,1),%eax
 80505c1:	89 44 24 04          	mov    %eax,0x4(%esp)
 80505c5:	8b 54 24 40          	mov    0x40(%esp),%edx
 80505c9:	89 14 24             	mov    %edx,(%esp)
 80505cc:	e8 a7 0d 00 00       	call   8051378 <__swbuf_r>
 80505d1:	40                   	inc    %eax
 80505d2:	0f 94 c2             	sete   %dl
 80505d5:	eb b0                	jmp    8050587 <_fputwc_r+0x97>
 80505d7:	90                   	nop
	FILE *fp)
{
  char buf[MB_LEN_MAX];
  size_t i, len;

  if (MB_CUR_MAX == 1 && wc > 0 && wc <= UCHAR_MAX)
 80505d8:	8b 44 24 44          	mov    0x44(%esp),%eax
 80505dc:	85 c0                	test   %eax,%eax
 80505de:	0f 8e 39 ff ff ff    	jle    805051d <_fputwc_r+0x2d>
 80505e4:	81 7c 24 44 ff 00 00 	cmpl   $0xff,0x44(%esp)
 80505eb:	00 
 80505ec:	0f 8f 2b ff ff ff    	jg     805051d <_fputwc_r+0x2d>
      /*
       * Assume single-byte locale with no special encoding.
       * A more careful test would be to check
       * _CurrentRuneLocale->encoding.
       */
      *buf = (unsigned char)wc;
 80505f2:	8a 44 24 44          	mov    0x44(%esp),%al
 80505f6:	88 44 24 1f          	mov    %al,0x1f(%esp)
      len = 1;
 80505fa:	bf 01 00 00 00       	mov    $0x1,%edi
 80505ff:	8d 6c 24 1f          	lea    0x1f(%esp),%ebp
 8050603:	e9 43 ff ff ff       	jmp    805054b <_fputwc_r+0x5b>
	  return WEOF;
	}
    }

  for (i = 0; i < len; i++)
    if (__sputc_r (ptr, (unsigned char) buf[i], fp) == EOF)
 8050608:	89 5c 24 08          	mov    %ebx,0x8(%esp)
 805060c:	c7 44 24 04 0a 00 00 	movl   $0xa,0x4(%esp)
 8050613:	00 
 8050614:	eb af                	jmp    80505c5 <_fputwc_r+0xd5>
 8050616:	66 90                	xchg   %ax,%ax

08050618 <fputwc>:

wint_t
_DEFUN(fputwc, (wc, fp),
	wchar_t wc _AND
	FILE *fp)
{
 8050618:	53                   	push   %ebx
 8050619:	83 ec 18             	sub    $0x18,%esp
  struct _reent *reent = _REENT;
 805061c:	8b 1d 40 40 05 08    	mov    0x8054040,%ebx

  CHECK_INIT(reent, fp);
 8050622:	85 db                	test   %ebx,%ebx
 8050624:	74 0f                	je     8050635 <fputwc+0x1d>
 8050626:	8b 53 38             	mov    0x38(%ebx),%edx
 8050629:	85 d2                	test   %edx,%edx
 805062b:	75 08                	jne    8050635 <fputwc+0x1d>
 805062d:	89 1c 24             	mov    %ebx,(%esp)
 8050630:	e8 2f c2 ff ff       	call   804c864 <__sinit>
  return _fputwc_r (reent, wc, fp);
 8050635:	8b 44 24 24          	mov    0x24(%esp),%eax
 8050639:	89 44 24 08          	mov    %eax,0x8(%esp)
 805063d:	8b 44 24 20          	mov    0x20(%esp),%eax
 8050641:	89 44 24 04          	mov    %eax,0x4(%esp)
 8050645:	89 1c 24             	mov    %ebx,(%esp)
 8050648:	e8 a3 fe ff ff       	call   80504f0 <_fputwc_r>
}
 805064d:	83 c4 18             	add    $0x18,%esp
 8050650:	5b                   	pop    %ebx
 8050651:	c3                   	ret    
 8050652:	90                   	nop
 8050653:	90                   	nop

08050654 <_fstat_r>:
int
_fstat_r (ptr, fd, pstat)
     struct _reent *ptr;
     int fd;
     struct stat *pstat;
{
 8050654:	83 ec 1c             	sub    $0x1c,%esp
  int ret;

  errno = 0;
 8050657:	c7 05 f4 49 05 08 00 	movl   $0x0,0x80549f4
 805065e:	00 00 00 
  if ((ret = _fstat (fd, pstat)) == -1 && errno != 0)
 8050661:	8b 44 24 28          	mov    0x28(%esp),%eax
 8050665:	89 44 24 04          	mov    %eax,0x4(%esp)
 8050669:	8b 44 24 24          	mov    0x24(%esp),%eax
 805066d:	89 04 24             	mov    %eax,(%esp)
 8050670:	e8 43 05 00 00       	call   8050bb8 <fstat>
 8050675:	83 f8 ff             	cmp    $0xffffffff,%eax
 8050678:	74 06                	je     8050680 <_fstat_r+0x2c>
    ptr->_errno = errno;
  return ret;
}
 805067a:	83 c4 1c             	add    $0x1c,%esp
 805067d:	c3                   	ret    
 805067e:	66 90                	xchg   %ax,%ax
     struct stat *pstat;
{
  int ret;

  errno = 0;
  if ((ret = _fstat (fd, pstat)) == -1 && errno != 0)
 8050680:	8b 15 f4 49 05 08    	mov    0x80549f4,%edx
 8050686:	85 d2                	test   %edx,%edx
 8050688:	74 f0                	je     805067a <_fstat_r+0x26>
    ptr->_errno = errno;
 805068a:	8b 4c 24 20          	mov    0x20(%esp),%ecx
 805068e:	89 11                	mov    %edx,(%ecx)
  return ret;
}
 8050690:	83 c4 1c             	add    $0x1c,%esp
 8050693:	c3                   	ret    

08050694 <_isatty_r>:

int
_isatty_r (ptr, fd)
     struct _reent *ptr;
     int fd;
{
 8050694:	83 ec 1c             	sub    $0x1c,%esp
  int ret;

  errno = 0;
 8050697:	c7 05 f4 49 05 08 00 	movl   $0x0,0x80549f4
 805069e:	00 00 00 
  if ((ret = _isatty (fd)) == -1 && errno != 0)
 80506a1:	8b 44 24 24          	mov    0x24(%esp),%eax
 80506a5:	89 04 24             	mov    %eax,(%esp)
 80506a8:	e8 5b 0f 00 00       	call   8051608 <isatty>
 80506ad:	83 f8 ff             	cmp    $0xffffffff,%eax
 80506b0:	74 06                	je     80506b8 <_isatty_r+0x24>
    ptr->_errno = errno;
  return ret;
}
 80506b2:	83 c4 1c             	add    $0x1c,%esp
 80506b5:	c3                   	ret    
 80506b6:	66 90                	xchg   %ax,%ax
     int fd;
{
  int ret;

  errno = 0;
  if ((ret = _isatty (fd)) == -1 && errno != 0)
 80506b8:	8b 15 f4 49 05 08    	mov    0x80549f4,%edx
 80506be:	85 d2                	test   %edx,%edx
 80506c0:	74 f0                	je     80506b2 <_isatty_r+0x1e>
    ptr->_errno = errno;
 80506c2:	8b 4c 24 20          	mov    0x20(%esp),%ecx
 80506c6:	89 11                	mov    %edx,(%ecx)
  return ret;
}
 80506c8:	83 c4 1c             	add    $0x1c,%esp
 80506cb:	c3                   	ret    

080506cc <_lseek_r>:
_DEFUN (_lseek_r, (ptr, fd, pos, whence),
     struct _reent *ptr _AND
     int fd _AND
     _off_t pos _AND
     int whence)
{
 80506cc:	83 ec 1c             	sub    $0x1c,%esp
  _off_t ret;

  errno = 0;
 80506cf:	c7 05 f4 49 05 08 00 	movl   $0x0,0x80549f4
 80506d6:	00 00 00 
  if ((ret = _lseek (fd, pos, whence)) == (_off_t) -1 && errno != 0)
 80506d9:	8b 44 24 2c          	mov    0x2c(%esp),%eax
 80506dd:	89 44 24 08          	mov    %eax,0x8(%esp)
 80506e1:	8b 44 24 28          	mov    0x28(%esp),%eax
 80506e5:	89 44 24 04          	mov    %eax,0x4(%esp)
 80506e9:	8b 44 24 24          	mov    0x24(%esp),%eax
 80506ed:	89 04 24             	mov    %eax,(%esp)
 80506f0:	e8 df 05 00 00       	call   8050cd4 <lseek>
 80506f5:	83 f8 ff             	cmp    $0xffffffff,%eax
 80506f8:	74 06                	je     8050700 <_lseek_r+0x34>
    ptr->_errno = errno;
  return ret;
}
 80506fa:	83 c4 1c             	add    $0x1c,%esp
 80506fd:	c3                   	ret    
 80506fe:	66 90                	xchg   %ax,%ax
     int whence)
{
  _off_t ret;

  errno = 0;
  if ((ret = _lseek (fd, pos, whence)) == (_off_t) -1 && errno != 0)
 8050700:	8b 15 f4 49 05 08    	mov    0x80549f4,%edx
 8050706:	85 d2                	test   %edx,%edx
 8050708:	74 f0                	je     80506fa <_lseek_r+0x2e>
    ptr->_errno = errno;
 805070a:	8b 4c 24 20          	mov    0x20(%esp),%ecx
 805070e:	89 11                	mov    %edx,(%ecx)
  return ret;
}
 8050710:	83 c4 1c             	add    $0x1c,%esp
 8050713:	c3                   	ret    

08050714 <_read_r>:
_DEFUN (_read_r, (ptr, fd, buf, cnt),
     struct _reent *ptr _AND
     int fd _AND
     _PTR buf _AND
     size_t cnt)
{
 8050714:	83 ec 1c             	sub    $0x1c,%esp
  _ssize_t ret;

  errno = 0;
 8050717:	c7 05 f4 49 05 08 00 	movl   $0x0,0x80549f4
 805071e:	00 00 00 
  if ((ret = (_ssize_t)_read (fd, buf, cnt)) == -1 && errno != 0)
 8050721:	8b 44 24 2c          	mov    0x2c(%esp),%eax
 8050725:	89 44 24 08          	mov    %eax,0x8(%esp)
 8050729:	8b 44 24 28          	mov    0x28(%esp),%eax
 805072d:	89 44 24 04          	mov    %eax,0x4(%esp)
 8050731:	8b 44 24 24          	mov    0x24(%esp),%eax
 8050735:	89 04 24             	mov    %eax,(%esp)
 8050738:	e8 5b 06 00 00       	call   8050d98 <read>
 805073d:	83 f8 ff             	cmp    $0xffffffff,%eax
 8050740:	74 06                	je     8050748 <_read_r+0x34>
    ptr->_errno = errno;
  return ret;
}
 8050742:	83 c4 1c             	add    $0x1c,%esp
 8050745:	c3                   	ret    
 8050746:	66 90                	xchg   %ax,%ax
     size_t cnt)
{
  _ssize_t ret;

  errno = 0;
  if ((ret = (_ssize_t)_read (fd, buf, cnt)) == -1 && errno != 0)
 8050748:	8b 15 f4 49 05 08    	mov    0x80549f4,%edx
 805074e:	85 d2                	test   %edx,%edx
 8050750:	74 f0                	je     8050742 <_read_r+0x2e>
    ptr->_errno = errno;
 8050752:	8b 4c 24 20          	mov    0x20(%esp),%ecx
 8050756:	89 11                	mov    %edx,(%ecx)
  return ret;
}
 8050758:	83 c4 1c             	add    $0x1c,%esp
 805075b:	c3                   	ret    

0805075c <cleanup_glue>:

void
_DEFUN (cleanup_glue, (ptr, glue),
     struct _reent *ptr _AND
     struct _glue *glue)
{
 805075c:	56                   	push   %esi
 805075d:	53                   	push   %ebx
 805075e:	83 ec 14             	sub    $0x14,%esp
 8050761:	8b 74 24 20          	mov    0x20(%esp),%esi
 8050765:	8b 5c 24 24          	mov    0x24(%esp),%ebx
  /* Have to reclaim these in reverse order: */
  if (glue->_next)
 8050769:	8b 03                	mov    (%ebx),%eax
 805076b:	85 c0                	test   %eax,%eax
 805076d:	74 0c                	je     805077b <cleanup_glue+0x1f>
    cleanup_glue (ptr, glue->_next);
 805076f:	89 44 24 04          	mov    %eax,0x4(%esp)
 8050773:	89 34 24             	mov    %esi,(%esp)
 8050776:	e8 e1 ff ff ff       	call   805075c <cleanup_glue>

  _free_r (ptr, glue);
 805077b:	89 5c 24 24          	mov    %ebx,0x24(%esp)
 805077f:	89 74 24 20          	mov    %esi,0x20(%esp)
}
 8050783:	83 c4 14             	add    $0x14,%esp
 8050786:	5b                   	pop    %ebx
 8050787:	5e                   	pop    %esi
{
  /* Have to reclaim these in reverse order: */
  if (glue->_next)
    cleanup_glue (ptr, glue->_next);

  _free_r (ptr, glue);
 8050788:	e9 73 c3 ff ff       	jmp    804cb00 <_free_r>
 805078d:	8d 76 00             	lea    0x0(%esi),%esi

08050790 <_reclaim_reent>:
}

void
_DEFUN (_reclaim_reent, (ptr),
     struct _reent *ptr)
{
 8050790:	57                   	push   %edi
 8050791:	56                   	push   %esi
 8050792:	53                   	push   %ebx
 8050793:	83 ec 10             	sub    $0x10,%esp
 8050796:	8b 74 24 20          	mov    0x20(%esp),%esi
  if (ptr != _impure_ptr)
 805079a:	3b 35 40 40 05 08    	cmp    0x8054040,%esi
 80507a0:	0f 84 98 00 00 00    	je     805083e <_reclaim_reent+0xae>
      /* used by mprec routines. */
#ifdef _REENT_SMALL
      if (ptr->_mp)	/* don't bother allocating it! */
      {
#endif
      if (_REENT_MP_FREELIST(ptr))
 80507a6:	8b 56 4c             	mov    0x4c(%esi),%edx
 80507a9:	85 d2                	test   %edx,%edx
 80507ab:	74 3a                	je     80507e7 <_reclaim_reent+0x57>
 80507ad:	31 c0                	xor    %eax,%eax
 80507af:	31 ff                	xor    %edi,%edi
 80507b1:	8d 76 00             	lea    0x0(%esi),%esi
	  int i;
	  for (i = 0; i < _Kmax; i++) 
	    {
	      struct _Bigint *thisone, *nextone;
	
	      nextone = _REENT_MP_FREELIST(ptr)[i];
 80507b4:	8b 04 82             	mov    (%edx,%eax,4),%eax
	      while (nextone)
 80507b7:	85 c0                	test   %eax,%eax
 80507b9:	74 18                	je     80507d3 <_reclaim_reent+0x43>
 80507bb:	90                   	nop
		{
		  thisone = nextone;
		  nextone = nextone->_next;
 80507bc:	8b 18                	mov    (%eax),%ebx
		  _free_r (ptr, thisone);
 80507be:	89 44 24 04          	mov    %eax,0x4(%esp)
 80507c2:	89 34 24             	mov    %esi,(%esp)
 80507c5:	e8 36 c3 ff ff       	call   804cb00 <_free_r>
	
	      nextone = _REENT_MP_FREELIST(ptr)[i];
	      while (nextone)
		{
		  thisone = nextone;
		  nextone = nextone->_next;
 80507ca:	89 d8                	mov    %ebx,%eax
	  for (i = 0; i < _Kmax; i++) 
	    {
	      struct _Bigint *thisone, *nextone;
	
	      nextone = _REENT_MP_FREELIST(ptr)[i];
	      while (nextone)
 80507cc:	85 db                	test   %ebx,%ebx
 80507ce:	75 ec                	jne    80507bc <_reclaim_reent+0x2c>
 80507d0:	8b 56 4c             	mov    0x4c(%esi),%edx
      {
#endif
      if (_REENT_MP_FREELIST(ptr))
	{
	  int i;
	  for (i = 0; i < _Kmax; i++) 
 80507d3:	47                   	inc    %edi
 80507d4:	89 f8                	mov    %edi,%eax
 80507d6:	83 ff 20             	cmp    $0x20,%edi
 80507d9:	75 d9                	jne    80507b4 <_reclaim_reent+0x24>
		  nextone = nextone->_next;
		  _free_r (ptr, thisone);
		}
	    }    

	  _free_r (ptr, _REENT_MP_FREELIST(ptr));
 80507db:	89 54 24 04          	mov    %edx,0x4(%esp)
 80507df:	89 34 24             	mov    %esi,(%esp)
 80507e2:	e8 19 c3 ff ff       	call   804cb00 <_free_r>
	}
      if (_REENT_MP_RESULT(ptr))
 80507e7:	8b 46 40             	mov    0x40(%esi),%eax
 80507ea:	85 c0                	test   %eax,%eax
 80507ec:	74 0c                	je     80507fa <_reclaim_reent+0x6a>
	_free_r (ptr, _REENT_MP_RESULT(ptr));
 80507ee:	89 44 24 04          	mov    %eax,0x4(%esp)
 80507f2:	89 34 24             	mov    %esi,(%esp)
 80507f5:	e8 06 c3 ff ff       	call   804cb00 <_free_r>
      /* atexit stuff */
# ifdef _REENT_SMALL
      if (ptr->_atexit && ptr->_atexit->_on_exit_args_ptr)
	_free_r (ptr, ptr->_atexit->_on_exit_args_ptr);
# else
      if ((ptr->_atexit) && (ptr->_atexit != &ptr->_atexit0))
 80507fa:	8b 86 48 01 00 00    	mov    0x148(%esi),%eax
 8050800:	85 c0                	test   %eax,%eax
 8050802:	74 20                	je     8050824 <_reclaim_reent+0x94>
 8050804:	8d be 4c 01 00 00    	lea    0x14c(%esi),%edi
 805080a:	39 f8                	cmp    %edi,%eax
 805080c:	74 16                	je     8050824 <_reclaim_reent+0x94>
 805080e:	66 90                	xchg   %ax,%ax
	{
	  struct _atexit *p, *q;
	  for (p = ptr->_atexit; p != &ptr->_atexit0;)
	    {
	      q = p;
	      p = p->_next;
 8050810:	8b 18                	mov    (%eax),%ebx
	      _free_r (ptr, q);
 8050812:	89 44 24 04          	mov    %eax,0x4(%esp)
 8050816:	89 34 24             	mov    %esi,(%esp)
 8050819:	e8 e2 c2 ff ff       	call   804cb00 <_free_r>
	{
	  struct _atexit *p, *q;
	  for (p = ptr->_atexit; p != &ptr->_atexit0;)
	    {
	      q = p;
	      p = p->_next;
 805081e:	89 d8                	mov    %ebx,%eax
	_free_r (ptr, ptr->_atexit->_on_exit_args_ptr);
# else
      if ((ptr->_atexit) && (ptr->_atexit != &ptr->_atexit0))
	{
	  struct _atexit *p, *q;
	  for (p = ptr->_atexit; p != &ptr->_atexit0;)
 8050820:	39 df                	cmp    %ebx,%edi
 8050822:	75 ec                	jne    8050810 <_reclaim_reent+0x80>
	    }
	}
# endif
#endif

      if (ptr->_cvtbuf)
 8050824:	8b 46 54             	mov    0x54(%esi),%eax
 8050827:	85 c0                	test   %eax,%eax
 8050829:	74 0c                	je     8050837 <_reclaim_reent+0xa7>
	_free_r (ptr, ptr->_cvtbuf);
 805082b:	89 44 24 04          	mov    %eax,0x4(%esp)
 805082f:	89 34 24             	mov    %esi,(%esp)
 8050832:	e8 c9 c2 ff ff       	call   804cb00 <_free_r>
	   do it safely considering that a signal may be delivered immediately
	   after the free?
	  if (ptr->_sig_func)
	_free_r (ptr, ptr->_sig_func);*/

      if (ptr->__sdidinit)
 8050837:	8b 46 38             	mov    0x38(%esi),%eax
 805083a:	85 c0                	test   %eax,%eax
 805083c:	75 07                	jne    8050845 <_reclaim_reent+0xb5>
	}

      /* Malloc memory not reclaimed; no good way to return memory anyway. */

    }
}
 805083e:	83 c4 10             	add    $0x10,%esp
 8050841:	5b                   	pop    %ebx
 8050842:	5e                   	pop    %esi
 8050843:	5f                   	pop    %edi
 8050844:	c3                   	ret    

      if (ptr->__sdidinit)
	{
	  /* cleanup won't reclaim memory 'coz usually it's run
	     before the program exits, and who wants to wait for that? */
	  ptr->__cleanup (ptr);
 8050845:	89 34 24             	mov    %esi,(%esp)
 8050848:	ff 56 3c             	call   *0x3c(%esi)

	  if (ptr->__sglue._next)
 805084b:	8b 86 e0 02 00 00    	mov    0x2e0(%esi),%eax
 8050851:	85 c0                	test   %eax,%eax
 8050853:	74 e9                	je     805083e <_reclaim_reent+0xae>
	    cleanup_glue (ptr, ptr->__sglue._next);
 8050855:	89 44 24 04          	mov    %eax,0x4(%esp)
 8050859:	89 34 24             	mov    %esi,(%esp)
 805085c:	e8 fb fe ff ff       	call   805075c <cleanup_glue>
	}

      /* Malloc memory not reclaimed; no good way to return memory anyway. */

    }
}
 8050861:	83 c4 10             	add    $0x10,%esp
 8050864:	5b                   	pop    %ebx
 8050865:	5e                   	pop    %esi
 8050866:	5f                   	pop    %edi
 8050867:	c3                   	ret    

08050868 <mkdir>:

int
_DEFUN (mkdir, (path, mode),
        const char *path  _AND
        mode_t mode)
{
 8050868:	83 ec 3c             	sub    $0x3c,%esp
 805086b:	8b 44 24 40          	mov    0x40(%esp),%eax
   int a[] = {(uint32_t) path, (uint32_t) mode, 0,0};
 805086f:	89 44 24 10          	mov    %eax,0x10(%esp)
 8050873:	8b 54 24 44          	mov    0x44(%esp),%edx
 8050877:	89 54 24 14          	mov    %edx,0x14(%esp)
 805087b:	c7 44 24 18 00 00 00 	movl   $0x0,0x18(%esp)
 8050882:	00 
 8050883:	c7 44 24 1c 00 00 00 	movl   $0x0,0x1c(%esp)
 805088a:	00 
   int b[] = {(uint32_t) 1+strlen(path),0,0,0};
 805088b:	89 04 24             	mov    %eax,(%esp)
 805088e:	e8 5d 83 ff ff       	call   8048bf0 <strlen>
 8050893:	40                   	inc    %eax
 8050894:	89 44 24 20          	mov    %eax,0x20(%esp)
 8050898:	c7 44 24 24 00 00 00 	movl   $0x0,0x24(%esp)
 805089f:	00 
 80508a0:	c7 44 24 28 00 00 00 	movl   $0x0,0x28(%esp)
 80508a7:	00 
 80508a8:	c7 44 24 2c 00 00 00 	movl   $0x0,0x2c(%esp)
 80508af:	00 
   return (int) nk_do_syscall(SYS_MKDIR, a, b); 
 80508b0:	8d 44 24 20          	lea    0x20(%esp),%eax
 80508b4:	89 44 24 08          	mov    %eax,0x8(%esp)
 80508b8:	8d 44 24 10          	lea    0x10(%esp),%eax
 80508bc:	89 44 24 04          	mov    %eax,0x4(%esp)
 80508c0:	c7 04 24 13 00 00 00 	movl   $0x13,(%esp)
 80508c7:	e8 cc e1 ff ff       	call   804ea98 <nk_do_syscall>
}
 80508cc:	83 c4 3c             	add    $0x3c,%esp
 80508cf:	c3                   	ret    

080508d0 <rmdir>:

int
_DEFUN (rmdir, (path),
        const char *path)
{
 80508d0:	83 ec 3c             	sub    $0x3c,%esp
 80508d3:	8b 44 24 40          	mov    0x40(%esp),%eax
   int a[] = {(uint32_t) path, 0, 0,0};
 80508d7:	89 44 24 10          	mov    %eax,0x10(%esp)
 80508db:	c7 44 24 14 00 00 00 	movl   $0x0,0x14(%esp)
 80508e2:	00 
 80508e3:	c7 44 24 18 00 00 00 	movl   $0x0,0x18(%esp)
 80508ea:	00 
 80508eb:	c7 44 24 1c 00 00 00 	movl   $0x0,0x1c(%esp)
 80508f2:	00 
   int b[] = {(uint32_t) 1+strlen(path),0,0,0};
 80508f3:	89 04 24             	mov    %eax,(%esp)
 80508f6:	e8 f5 82 ff ff       	call   8048bf0 <strlen>
 80508fb:	40                   	inc    %eax
 80508fc:	89 44 24 20          	mov    %eax,0x20(%esp)
 8050900:	c7 44 24 24 00 00 00 	movl   $0x0,0x24(%esp)
 8050907:	00 
 8050908:	c7 44 24 28 00 00 00 	movl   $0x0,0x28(%esp)
 805090f:	00 
 8050910:	c7 44 24 2c 00 00 00 	movl   $0x0,0x2c(%esp)
 8050917:	00 
   return (int) nk_do_syscall(SYS_RMDIR, a, b); 
 8050918:	8d 44 24 20          	lea    0x20(%esp),%eax
 805091c:	89 44 24 08          	mov    %eax,0x8(%esp)
 8050920:	8d 44 24 10          	lea    0x10(%esp),%eax
 8050924:	89 44 24 04          	mov    %eax,0x4(%esp)
 8050928:	c7 04 24 22 00 00 00 	movl   $0x22,(%esp)
 805092f:	e8 64 e1 ff ff       	call   804ea98 <nk_do_syscall>
}
 8050934:	83 c4 3c             	add    $0x3c,%esp
 8050937:	c3                   	ret    

08050938 <chown>:
int
_DEFUN (chown, (path, owner, group),
        const char *path  _AND
        uid_t owner _AND
        gid_t group)
{
 8050938:	83 ec 3c             	sub    $0x3c,%esp
 805093b:	8b 44 24 40          	mov    0x40(%esp),%eax
 805093f:	0f b7 4c 24 44       	movzwl 0x44(%esp),%ecx
 8050944:	0f b7 54 24 48       	movzwl 0x48(%esp),%edx
   int a[] = {(uint32_t) path, (uint32_t) owner, (uint32_t) group,0};
 8050949:	89 44 24 10          	mov    %eax,0x10(%esp)
 805094d:	89 4c 24 14          	mov    %ecx,0x14(%esp)
 8050951:	89 54 24 18          	mov    %edx,0x18(%esp)
 8050955:	c7 44 24 1c 00 00 00 	movl   $0x0,0x1c(%esp)
 805095c:	00 
   int b[] = {(uint32_t) 1+strlen(path),0,0,0};
 805095d:	89 04 24             	mov    %eax,(%esp)
 8050960:	e8 8b 82 ff ff       	call   8048bf0 <strlen>
 8050965:	40                   	inc    %eax
 8050966:	89 44 24 20          	mov    %eax,0x20(%esp)
 805096a:	c7 44 24 24 00 00 00 	movl   $0x0,0x24(%esp)
 8050971:	00 
 8050972:	c7 44 24 28 00 00 00 	movl   $0x0,0x28(%esp)
 8050979:	00 
 805097a:	c7 44 24 2c 00 00 00 	movl   $0x0,0x2c(%esp)
 8050981:	00 
   return (int) nk_do_syscall(SYS_CHOWN, a, b); 
 8050982:	8d 44 24 20          	lea    0x20(%esp),%eax
 8050986:	89 44 24 08          	mov    %eax,0x8(%esp)
 805098a:	8d 44 24 10          	lea    0x10(%esp),%eax
 805098e:	89 44 24 04          	mov    %eax,0x4(%esp)
 8050992:	c7 04 24 10 00 00 00 	movl   $0x10,(%esp)
 8050999:	e8 fa e0 ff ff       	call   804ea98 <nk_do_syscall>
}
 805099e:	83 c4 3c             	add    $0x3c,%esp
 80509a1:	c3                   	ret    
 80509a2:	66 90                	xchg   %ax,%ax

080509a4 <lchown>:
int
_DEFUN (lchown, (path, owner, group),
        const char *path  _AND
        uid_t owner _AND
        gid_t group)
{
 80509a4:	83 ec 3c             	sub    $0x3c,%esp
 80509a7:	8b 44 24 40          	mov    0x40(%esp),%eax
 80509ab:	0f b7 4c 24 44       	movzwl 0x44(%esp),%ecx
 80509b0:	0f b7 54 24 48       	movzwl 0x48(%esp),%edx
   int a[] = {(uint32_t) path, (uint32_t) owner, (uint32_t) group,0};
 80509b5:	89 44 24 10          	mov    %eax,0x10(%esp)
 80509b9:	89 4c 24 14          	mov    %ecx,0x14(%esp)
 80509bd:	89 54 24 18          	mov    %edx,0x18(%esp)
 80509c1:	c7 44 24 1c 00 00 00 	movl   $0x0,0x1c(%esp)
 80509c8:	00 
   int b[] = {(uint32_t) 1+strlen(path),0,0,0};
 80509c9:	89 04 24             	mov    %eax,(%esp)
 80509cc:	e8 1f 82 ff ff       	call   8048bf0 <strlen>
 80509d1:	40                   	inc    %eax
 80509d2:	89 44 24 20          	mov    %eax,0x20(%esp)
 80509d6:	c7 44 24 24 00 00 00 	movl   $0x0,0x24(%esp)
 80509dd:	00 
 80509de:	c7 44 24 28 00 00 00 	movl   $0x0,0x28(%esp)
 80509e5:	00 
 80509e6:	c7 44 24 2c 00 00 00 	movl   $0x0,0x2c(%esp)
 80509ed:	00 
   return (int) nk_do_syscall(SYS_CHOWN, a, b); 
 80509ee:	8d 44 24 20          	lea    0x20(%esp),%eax
 80509f2:	89 44 24 08          	mov    %eax,0x8(%esp)
 80509f6:	8d 44 24 10          	lea    0x10(%esp),%eax
 80509fa:	89 44 24 04          	mov    %eax,0x4(%esp)
 80509fe:	c7 04 24 10 00 00 00 	movl   $0x10,(%esp)
 8050a05:	e8 8e e0 ff ff       	call   804ea98 <nk_do_syscall>
}
 8050a0a:	83 c4 3c             	add    $0x3c,%esp
 8050a0d:	c3                   	ret    
 8050a0e:	66 90                	xchg   %ax,%ax

08050a10 <sync>:

void sync(){};
 8050a10:	c3                   	ret    
 8050a11:	8d 76 00             	lea    0x0(%esi),%esi

08050a14 <pipe2>:

int
_DEFUN (pipe2, (pipefd, flags),
        int pipefd[2] _AND
        int   flags)
{
 8050a14:	57                   	push   %edi
 8050a15:	83 ec 38             	sub    $0x38,%esp
   int a[] = {(uint32_t) pipefd, (uint32_t) flags,0,0};
 8050a18:	8b 44 24 40          	mov    0x40(%esp),%eax
 8050a1c:	89 44 24 10          	mov    %eax,0x10(%esp)
 8050a20:	8b 44 24 44          	mov    0x44(%esp),%eax
 8050a24:	89 44 24 14          	mov    %eax,0x14(%esp)
 8050a28:	c7 44 24 18 00 00 00 	movl   $0x0,0x18(%esp)
 8050a2f:	00 
 8050a30:	c7 44 24 1c 00 00 00 	movl   $0x0,0x1c(%esp)
 8050a37:	00 
   int b[] = {(uint32_t) 0,0,0,0};
 8050a38:	8d 7c 24 20          	lea    0x20(%esp),%edi
 8050a3c:	b9 10 00 00 00       	mov    $0x10,%ecx
 8050a41:	31 c0                	xor    %eax,%eax
 8050a43:	f3 aa                	rep stos %al,%es:(%edi)
   return (int) nk_do_syscall(SYS_PIPE2, a, b); 
 8050a45:	8d 44 24 20          	lea    0x20(%esp),%eax
 8050a49:	89 44 24 08          	mov    %eax,0x8(%esp)
 8050a4d:	8d 44 24 10          	lea    0x10(%esp),%eax
 8050a51:	89 44 24 04          	mov    %eax,0x4(%esp)
 8050a55:	c7 04 24 1d 00 00 00 	movl   $0x1d,(%esp)
 8050a5c:	e8 37 e0 ff ff       	call   804ea98 <nk_do_syscall>
}
 8050a61:	83 c4 38             	add    $0x38,%esp
 8050a64:	5f                   	pop    %edi
 8050a65:	c3                   	ret    
 8050a66:	66 90                	xchg   %ax,%ax

08050a68 <pipe>:

int
_DEFUN (pipe, (pipefd),
        int pipefd[2])
{
 8050a68:	57                   	push   %edi
 8050a69:	83 ec 38             	sub    $0x38,%esp
   int a[] = {(uint32_t) pipefd,0,0,0};
 8050a6c:	8b 44 24 40          	mov    0x40(%esp),%eax
 8050a70:	89 44 24 10          	mov    %eax,0x10(%esp)
 8050a74:	c7 44 24 14 00 00 00 	movl   $0x0,0x14(%esp)
 8050a7b:	00 
 8050a7c:	c7 44 24 18 00 00 00 	movl   $0x0,0x18(%esp)
 8050a83:	00 
 8050a84:	c7 44 24 1c 00 00 00 	movl   $0x0,0x1c(%esp)
 8050a8b:	00 
   int b[] = {(uint32_t) 0,0,0,0};
 8050a8c:	8d 7c 24 20          	lea    0x20(%esp),%edi
 8050a90:	b9 10 00 00 00       	mov    $0x10,%ecx
 8050a95:	31 c0                	xor    %eax,%eax
 8050a97:	f3 aa                	rep stos %al,%es:(%edi)
   return (int) nk_do_syscall(SYS_PIPE, a, b); 
 8050a99:	8d 44 24 20          	lea    0x20(%esp),%eax
 8050a9d:	89 44 24 08          	mov    %eax,0x8(%esp)
 8050aa1:	8d 44 24 10          	lea    0x10(%esp),%eax
 8050aa5:	89 44 24 04          	mov    %eax,0x4(%esp)
 8050aa9:	c7 04 24 1e 00 00 00 	movl   $0x1e,(%esp)
 8050ab0:	e8 e3 df ff ff       	call   804ea98 <nk_do_syscall>
}
 8050ab5:	83 c4 38             	add    $0x38,%esp
 8050ab8:	5f                   	pop    %edi
 8050ab9:	c3                   	ret    
 8050aba:	66 90                	xchg   %ax,%ax

08050abc <dup>:

int
_DEFUN (dup, (old),
        int old)
{
 8050abc:	57                   	push   %edi
 8050abd:	83 ec 38             	sub    $0x38,%esp
   int a[] = {(uint32_t) old, 0,0,0};
 8050ac0:	8b 44 24 40          	mov    0x40(%esp),%eax
 8050ac4:	89 44 24 10          	mov    %eax,0x10(%esp)
 8050ac8:	c7 44 24 14 00 00 00 	movl   $0x0,0x14(%esp)
 8050acf:	00 
 8050ad0:	c7 44 24 18 00 00 00 	movl   $0x0,0x18(%esp)
 8050ad7:	00 
 8050ad8:	c7 44 24 1c 00 00 00 	movl   $0x0,0x1c(%esp)
 8050adf:	00 
   int b[] = {(uint32_t) 0,0,0,0};
 8050ae0:	8d 7c 24 20          	lea    0x20(%esp),%edi
 8050ae4:	b9 10 00 00 00       	mov    $0x10,%ecx
 8050ae9:	31 c0                	xor    %eax,%eax
 8050aeb:	f3 aa                	rep stos %al,%es:(%edi)
   return (int) nk_do_syscall(SYS_DUP, a, b); 
 8050aed:	8d 44 24 20          	lea    0x20(%esp),%eax
 8050af1:	89 44 24 08          	mov    %eax,0x8(%esp)
 8050af5:	8d 44 24 10          	lea    0x10(%esp),%eax
 8050af9:	89 44 24 04          	mov    %eax,0x4(%esp)
 8050afd:	c7 04 24 1a 00 00 00 	movl   $0x1a,(%esp)
 8050b04:	e8 8f df ff ff       	call   804ea98 <nk_do_syscall>
}
 8050b09:	83 c4 38             	add    $0x38,%esp
 8050b0c:	5f                   	pop    %edi
 8050b0d:	c3                   	ret    
 8050b0e:	66 90                	xchg   %ax,%ax

08050b10 <dup2>:

int
_DEFUN (dup2, (old,new),
        int old _AND
	int new)
{
 8050b10:	57                   	push   %edi
 8050b11:	83 ec 38             	sub    $0x38,%esp
   int a[] = {(uint32_t) old, (uint32_t) new,0,0};
 8050b14:	8b 44 24 40          	mov    0x40(%esp),%eax
 8050b18:	89 44 24 10          	mov    %eax,0x10(%esp)
 8050b1c:	8b 44 24 44          	mov    0x44(%esp),%eax
 8050b20:	89 44 24 14          	mov    %eax,0x14(%esp)
 8050b24:	c7 44 24 18 00 00 00 	movl   $0x0,0x18(%esp)
 8050b2b:	00 
 8050b2c:	c7 44 24 1c 00 00 00 	movl   $0x0,0x1c(%esp)
 8050b33:	00 
   int b[] = {(uint32_t) 0,0,0,0};
 8050b34:	8d 7c 24 20          	lea    0x20(%esp),%edi
 8050b38:	b9 10 00 00 00       	mov    $0x10,%ecx
 8050b3d:	31 c0                	xor    %eax,%eax
 8050b3f:	f3 aa                	rep stos %al,%es:(%edi)
   return (int) nk_do_syscall(SYS_DUP2, a, b); 
 8050b41:	8d 44 24 20          	lea    0x20(%esp),%eax
 8050b45:	89 44 24 08          	mov    %eax,0x8(%esp)
 8050b49:	8d 44 24 10          	lea    0x10(%esp),%eax
 8050b4d:	89 44 24 04          	mov    %eax,0x4(%esp)
 8050b51:	c7 04 24 19 00 00 00 	movl   $0x19,(%esp)
 8050b58:	e8 3b df ff ff       	call   804ea98 <nk_do_syscall>
}
 8050b5d:	83 c4 38             	add    $0x38,%esp
 8050b60:	5f                   	pop    %edi
 8050b61:	c3                   	ret    
 8050b62:	66 90                	xchg   %ax,%ax

08050b64 <close>:

int
_DEFUN (close, (fildes),
        int fildes)
{
 8050b64:	57                   	push   %edi
 8050b65:	83 ec 38             	sub    $0x38,%esp
   int a[] = {(uint32_t) fildes, 0,0,0};
 8050b68:	8b 44 24 40          	mov    0x40(%esp),%eax
 8050b6c:	89 44 24 10          	mov    %eax,0x10(%esp)
 8050b70:	c7 44 24 14 00 00 00 	movl   $0x0,0x14(%esp)
 8050b77:	00 
 8050b78:	c7 44 24 18 00 00 00 	movl   $0x0,0x18(%esp)
 8050b7f:	00 
 8050b80:	c7 44 24 1c 00 00 00 	movl   $0x0,0x1c(%esp)
 8050b87:	00 
   int b[] = {0,0,0,0};
 8050b88:	8d 7c 24 20          	lea    0x20(%esp),%edi
 8050b8c:	b9 10 00 00 00       	mov    $0x10,%ecx
 8050b91:	31 c0                	xor    %eax,%eax
 8050b93:	f3 aa                	rep stos %al,%es:(%edi)
   return (int) nk_do_syscall(SYS_CLOSE, a, b); 
 8050b95:	8d 44 24 20          	lea    0x20(%esp),%eax
 8050b99:	89 44 24 08          	mov    %eax,0x8(%esp)
 8050b9d:	8d 44 24 10          	lea    0x10(%esp),%eax
 8050ba1:	89 44 24 04          	mov    %eax,0x4(%esp)
 8050ba5:	c7 04 24 1c 00 00 00 	movl   $0x1c,(%esp)
 8050bac:	e8 e7 de ff ff       	call   804ea98 <nk_do_syscall>
}
 8050bb1:	83 c4 38             	add    $0x38,%esp
 8050bb4:	5f                   	pop    %edi
 8050bb5:	c3                   	ret    
 8050bb6:	66 90                	xchg   %ax,%ax

08050bb8 <fstat>:

int
_DEFUN (fstat, (fildes, st),
        int          fildes _AND
        struct stat *st)
{
 8050bb8:	57                   	push   %edi
 8050bb9:	83 ec 38             	sub    $0x38,%esp
   int a[] = {(uint32_t) fildes, (uint32_t) st,0,0};
 8050bbc:	8b 44 24 40          	mov    0x40(%esp),%eax
 8050bc0:	89 44 24 10          	mov    %eax,0x10(%esp)
 8050bc4:	8b 44 24 44          	mov    0x44(%esp),%eax
 8050bc8:	89 44 24 14          	mov    %eax,0x14(%esp)
 8050bcc:	c7 44 24 18 00 00 00 	movl   $0x0,0x18(%esp)
 8050bd3:	00 
 8050bd4:	c7 44 24 1c 00 00 00 	movl   $0x0,0x1c(%esp)
 8050bdb:	00 
   int b[] = {0,0,0,0};
 8050bdc:	8d 7c 24 20          	lea    0x20(%esp),%edi
 8050be0:	b9 10 00 00 00       	mov    $0x10,%ecx
 8050be5:	31 c0                	xor    %eax,%eax
 8050be7:	f3 aa                	rep stos %al,%es:(%edi)
   return (int) nk_do_syscall(SYS_FSTAT, a, b); 
 8050be9:	8d 44 24 20          	lea    0x20(%esp),%eax
 8050bed:	89 44 24 08          	mov    %eax,0x8(%esp)
 8050bf1:	8d 44 24 10          	lea    0x10(%esp),%eax
 8050bf5:	89 44 24 04          	mov    %eax,0x4(%esp)
 8050bf9:	c7 04 24 18 00 00 00 	movl   $0x18,(%esp)
 8050c00:	e8 93 de ff ff       	call   804ea98 <nk_do_syscall>
}
 8050c05:	83 c4 38             	add    $0x38,%esp
 8050c08:	5f                   	pop    %edi
 8050c09:	c3                   	ret    
 8050c0a:	66 90                	xchg   %ax,%ax

08050c0c <_isatty>:

int
_DEFUN (_isatty, (file),
        int file)
{
 8050c0c:	83 ec 5c             	sub    $0x5c,%esp
  struct stat buf;

  if (fstat (file, &buf) < 0) {
 8050c0f:	8d 44 24 14          	lea    0x14(%esp),%eax
 8050c13:	89 44 24 04          	mov    %eax,0x4(%esp)
 8050c17:	8b 44 24 60          	mov    0x60(%esp),%eax
 8050c1b:	89 04 24             	mov    %eax,(%esp)
 8050c1e:	e8 95 ff ff ff       	call   8050bb8 <fstat>
 8050c23:	85 c0                	test   %eax,%eax
 8050c25:	78 2d                	js     8050c54 <_isatty+0x48>
    errno = EBADF;
    return 0;
  }
  if (S_ISCHR (buf.st_mode))
 8050c27:	8b 44 24 18          	mov    0x18(%esp),%eax
 8050c2b:	25 00 f0 00 00       	and    $0xf000,%eax
 8050c30:	3d 00 20 00 00       	cmp    $0x2000,%eax
 8050c35:	74 11                	je     8050c48 <_isatty+0x3c>
    return 1;
  errno = ENOTTY;
 8050c37:	e8 c4 b8 ff ff       	call   804c500 <__errno>
 8050c3c:	c7 00 19 00 00 00    	movl   $0x19,(%eax)
  return 0;
 8050c42:	31 c0                	xor    %eax,%eax
}
 8050c44:	83 c4 5c             	add    $0x5c,%esp
 8050c47:	c3                   	ret    
  if (fstat (file, &buf) < 0) {
    errno = EBADF;
    return 0;
  }
  if (S_ISCHR (buf.st_mode))
    return 1;
 8050c48:	b8 01 00 00 00       	mov    $0x1,%eax
  errno = ENOTTY;
  return 0;
}
 8050c4d:	83 c4 5c             	add    $0x5c,%esp
 8050c50:	c3                   	ret    
 8050c51:	8d 76 00             	lea    0x0(%esi),%esi
        int file)
{
  struct stat buf;

  if (fstat (file, &buf) < 0) {
    errno = EBADF;
 8050c54:	e8 a7 b8 ff ff       	call   804c500 <__errno>
 8050c59:	c7 00 09 00 00 00    	movl   $0x9,(%eax)
    return 0;
 8050c5f:	31 c0                	xor    %eax,%eax
 8050c61:	eb e1                	jmp    8050c44 <_isatty+0x38>
 8050c63:	90                   	nop

08050c64 <link>:

int
_DEFUN (link, (existing, new),
        char *existing _AND
        char *new)
{
 8050c64:	53                   	push   %ebx
 8050c65:	83 ec 38             	sub    $0x38,%esp
 8050c68:	8b 44 24 40          	mov    0x40(%esp),%eax
 8050c6c:	8b 5c 24 44          	mov    0x44(%esp),%ebx
   int a[] = {(uint32_t) existing, (uint32_t) new, 0,0};
 8050c70:	89 44 24 10          	mov    %eax,0x10(%esp)
 8050c74:	89 5c 24 14          	mov    %ebx,0x14(%esp)
 8050c78:	c7 44 24 18 00 00 00 	movl   $0x0,0x18(%esp)
 8050c7f:	00 
 8050c80:	c7 44 24 1c 00 00 00 	movl   $0x0,0x1c(%esp)
 8050c87:	00 
   int b[] = {(uint32_t) 1+strlen(existing), 1+strlen(new),0,0};
 8050c88:	89 04 24             	mov    %eax,(%esp)
 8050c8b:	e8 60 7f ff ff       	call   8048bf0 <strlen>
 8050c90:	40                   	inc    %eax
 8050c91:	89 44 24 20          	mov    %eax,0x20(%esp)
 8050c95:	89 1c 24             	mov    %ebx,(%esp)
 8050c98:	e8 53 7f ff ff       	call   8048bf0 <strlen>
 8050c9d:	40                   	inc    %eax
 8050c9e:	89 44 24 24          	mov    %eax,0x24(%esp)
 8050ca2:	c7 44 24 28 00 00 00 	movl   $0x0,0x28(%esp)
 8050ca9:	00 
 8050caa:	c7 44 24 2c 00 00 00 	movl   $0x0,0x2c(%esp)
 8050cb1:	00 
   return (int) nk_do_syscall(SYS_LINK, a, b); 
 8050cb2:	8d 44 24 20          	lea    0x20(%esp),%eax
 8050cb6:	89 44 24 08          	mov    %eax,0x8(%esp)
 8050cba:	8d 44 24 10          	lea    0x10(%esp),%eax
 8050cbe:	89 44 24 04          	mov    %eax,0x4(%esp)
 8050cc2:	c7 04 24 0d 00 00 00 	movl   $0xd,(%esp)
 8050cc9:	e8 ca dd ff ff       	call   804ea98 <nk_do_syscall>
}
 8050cce:	83 c4 38             	add    $0x38,%esp
 8050cd1:	5b                   	pop    %ebx
 8050cd2:	c3                   	ret    
 8050cd3:	90                   	nop

08050cd4 <lseek>:
int
_DEFUN (lseek, (file, ptr, dir),
        int   file  _AND
        int   ptr   _AND
        int   dir)
{
 8050cd4:	57                   	push   %edi
 8050cd5:	83 ec 38             	sub    $0x38,%esp
   int a[] = {(uint32_t) file, (uint32_t) ptr, (uint32_t) dir,0};
 8050cd8:	8b 44 24 40          	mov    0x40(%esp),%eax
 8050cdc:	89 44 24 10          	mov    %eax,0x10(%esp)
 8050ce0:	8b 44 24 44          	mov    0x44(%esp),%eax
 8050ce4:	89 44 24 14          	mov    %eax,0x14(%esp)
 8050ce8:	8b 44 24 48          	mov    0x48(%esp),%eax
 8050cec:	89 44 24 18          	mov    %eax,0x18(%esp)
 8050cf0:	c7 44 24 1c 00 00 00 	movl   $0x0,0x1c(%esp)
 8050cf7:	00 
   int b[] = {0, 0, 0, 0};
 8050cf8:	8d 7c 24 20          	lea    0x20(%esp),%edi
 8050cfc:	b9 10 00 00 00       	mov    $0x10,%ecx
 8050d01:	31 c0                	xor    %eax,%eax
 8050d03:	f3 aa                	rep stos %al,%es:(%edi)
   return (int) nk_do_syscall(SYS_LSEEK, a, b); 
 8050d05:	8d 44 24 20          	lea    0x20(%esp),%eax
 8050d09:	89 44 24 08          	mov    %eax,0x8(%esp)
 8050d0d:	8d 44 24 10          	lea    0x10(%esp),%eax
 8050d11:	89 44 24 04          	mov    %eax,0x4(%esp)
 8050d15:	c7 04 24 21 00 00 00 	movl   $0x21,(%esp)
 8050d1c:	e8 77 dd ff ff       	call   804ea98 <nk_do_syscall>
}
 8050d21:	83 c4 38             	add    $0x38,%esp
 8050d24:	5f                   	pop    %edi
 8050d25:	c3                   	ret    
 8050d26:	66 90                	xchg   %ax,%ax

08050d28 <open>:
int
_DEFUN (open, (file, flags, mode),
        char *file  _AND
        int   flags _AND
        int   mode)
{
 8050d28:	83 ec 3c             	sub    $0x3c,%esp
 8050d2b:	8b 44 24 40          	mov    0x40(%esp),%eax
   uint32_t a[] = {(uint32_t) file, (uint32_t) flags, (uint32_t) mode,0};
 8050d2f:	89 44 24 10          	mov    %eax,0x10(%esp)
 8050d33:	8b 54 24 44          	mov    0x44(%esp),%edx
 8050d37:	89 54 24 14          	mov    %edx,0x14(%esp)
 8050d3b:	8b 54 24 48          	mov    0x48(%esp),%edx
 8050d3f:	89 54 24 18          	mov    %edx,0x18(%esp)
 8050d43:	c7 44 24 1c 00 00 00 	movl   $0x0,0x1c(%esp)
 8050d4a:	00 
   uint32_t b[] = {(uint32_t) 1+strlen(file), 0, 0, 0};
 8050d4b:	89 04 24             	mov    %eax,(%esp)
 8050d4e:	e8 9d 7e ff ff       	call   8048bf0 <strlen>
 8050d53:	40                   	inc    %eax
 8050d54:	89 44 24 20          	mov    %eax,0x20(%esp)
 8050d58:	c7 44 24 24 00 00 00 	movl   $0x0,0x24(%esp)
 8050d5f:	00 
 8050d60:	c7 44 24 28 00 00 00 	movl   $0x0,0x28(%esp)
 8050d67:	00 
 8050d68:	c7 44 24 2c 00 00 00 	movl   $0x0,0x2c(%esp)
 8050d6f:	00 
   return (int) nk_do_syscall(SYS_OPEN, a, b); 
 8050d70:	8d 44 24 20          	lea    0x20(%esp),%eax
 8050d74:	89 44 24 08          	mov    %eax,0x8(%esp)
 8050d78:	8d 44 24 10          	lea    0x10(%esp),%eax
 8050d7c:	89 44 24 04          	mov    %eax,0x4(%esp)
 8050d80:	c7 04 24 1b 00 00 00 	movl   $0x1b,(%esp)
 8050d87:	e8 0c dd ff ff       	call   804ea98 <nk_do_syscall>
}
 8050d8c:	83 c4 3c             	add    $0x3c,%esp
 8050d8f:	c3                   	ret    

08050d90 <utime>:

int utime(const char *filename, const struct utimbuf *times)
{
	return 0;
}
 8050d90:	31 c0                	xor    %eax,%eax
 8050d92:	c3                   	ret    
 8050d93:	90                   	nop

08050d94 <utimes>:

int utimes(const char *filename, const struct timeval times[2])
{
	return 0;
}
 8050d94:	31 c0                	xor    %eax,%eax
 8050d96:	c3                   	ret    
 8050d97:	90                   	nop

08050d98 <read>:
int
_DEFUN (read, (file, ptr, len),
        int   file  _AND
        char *ptr   _AND
        int   len)
{
 8050d98:	57                   	push   %edi
 8050d99:	83 ec 38             	sub    $0x38,%esp
   int a[] = {(uint32_t) file, (uint32_t) ptr, (uint32_t) len,0};
 8050d9c:	8b 44 24 40          	mov    0x40(%esp),%eax
 8050da0:	89 44 24 10          	mov    %eax,0x10(%esp)
 8050da4:	8b 44 24 44          	mov    0x44(%esp),%eax
 8050da8:	89 44 24 14          	mov    %eax,0x14(%esp)
 8050dac:	8b 44 24 48          	mov    0x48(%esp),%eax
 8050db0:	89 44 24 18          	mov    %eax,0x18(%esp)
 8050db4:	c7 44 24 1c 00 00 00 	movl   $0x0,0x1c(%esp)
 8050dbb:	00 
   int b[] = {0, 0, 0, 0};
 8050dbc:	8d 7c 24 20          	lea    0x20(%esp),%edi
 8050dc0:	b9 10 00 00 00       	mov    $0x10,%ecx
 8050dc5:	31 c0                	xor    %eax,%eax
 8050dc7:	f3 aa                	rep stos %al,%es:(%edi)
   return (int) nk_do_syscall(SYS_READ, a, b); 
 8050dc9:	8d 44 24 20          	lea    0x20(%esp),%eax
 8050dcd:	89 44 24 08          	mov    %eax,0x8(%esp)
 8050dd1:	8d 44 24 10          	lea    0x10(%esp),%eax
 8050dd5:	89 44 24 04          	mov    %eax,0x4(%esp)
 8050dd9:	c7 04 24 1f 00 00 00 	movl   $0x1f,(%esp)
 8050de0:	e8 b3 dc ff ff       	call   804ea98 <nk_do_syscall>
}
 8050de5:	83 c4 38             	add    $0x38,%esp
 8050de8:	5f                   	pop    %edi
 8050de9:	c3                   	ret    
 8050dea:	66 90                	xchg   %ax,%ax

08050dec <readlink>:
int
_DEFUN (readlink, (path, buf, bufsize),
        const char *path _AND
        char *buf _AND
        size_t bufsize)
{
 8050dec:	83 ec 3c             	sub    $0x3c,%esp
 8050def:	8b 44 24 40          	mov    0x40(%esp),%eax
   int a[] = {(uint32_t) path, (uint32_t) buf, (uint32_t) bufsize,0};
 8050df3:	89 44 24 10          	mov    %eax,0x10(%esp)
 8050df7:	8b 54 24 44          	mov    0x44(%esp),%edx
 8050dfb:	89 54 24 14          	mov    %edx,0x14(%esp)
 8050dff:	8b 54 24 48          	mov    0x48(%esp),%edx
 8050e03:	89 54 24 18          	mov    %edx,0x18(%esp)
 8050e07:	c7 44 24 1c 00 00 00 	movl   $0x0,0x1c(%esp)
 8050e0e:	00 
   int b[] = {(uint32_t) 1+strlen(path),0,0,0};
 8050e0f:	89 04 24             	mov    %eax,(%esp)
 8050e12:	e8 d9 7d ff ff       	call   8048bf0 <strlen>
 8050e17:	40                   	inc    %eax
 8050e18:	89 44 24 20          	mov    %eax,0x20(%esp)
 8050e1c:	c7 44 24 24 00 00 00 	movl   $0x0,0x24(%esp)
 8050e23:	00 
 8050e24:	c7 44 24 28 00 00 00 	movl   $0x0,0x28(%esp)
 8050e2b:	00 
 8050e2c:	c7 44 24 2c 00 00 00 	movl   $0x0,0x2c(%esp)
 8050e33:	00 
   return (int) nk_do_syscall(SYS_READLINK, a, b); 
 8050e34:	8d 44 24 20          	lea    0x20(%esp),%eax
 8050e38:	89 44 24 08          	mov    %eax,0x8(%esp)
 8050e3c:	8d 44 24 10          	lea    0x10(%esp),%eax
 8050e40:	89 44 24 04          	mov    %eax,0x4(%esp)
 8050e44:	c7 04 24 28 00 00 00 	movl   $0x28,(%esp)
 8050e4b:	e8 48 dc ff ff       	call   804ea98 <nk_do_syscall>
}
 8050e50:	83 c4 3c             	add    $0x3c,%esp
 8050e53:	c3                   	ret    

08050e54 <stat>:

int
_DEFUN (stat, (file, st),
        const char  *file _AND
        struct stat *st)
{
 8050e54:	83 ec 3c             	sub    $0x3c,%esp
 8050e57:	8b 44 24 40          	mov    0x40(%esp),%eax
   int a[] = {(uint32_t) file, (uint32_t) st,0,0};
 8050e5b:	89 44 24 10          	mov    %eax,0x10(%esp)
 8050e5f:	8b 54 24 44          	mov    0x44(%esp),%edx
 8050e63:	89 54 24 14          	mov    %edx,0x14(%esp)
 8050e67:	c7 44 24 18 00 00 00 	movl   $0x0,0x18(%esp)
 8050e6e:	00 
 8050e6f:	c7 44 24 1c 00 00 00 	movl   $0x0,0x1c(%esp)
 8050e76:	00 
   int b[] = {(uint32_t) 1+strlen(file),0,0,0};
 8050e77:	89 04 24             	mov    %eax,(%esp)
 8050e7a:	e8 71 7d ff ff       	call   8048bf0 <strlen>
 8050e7f:	40                   	inc    %eax
 8050e80:	89 44 24 20          	mov    %eax,0x20(%esp)
 8050e84:	c7 44 24 24 00 00 00 	movl   $0x0,0x24(%esp)
 8050e8b:	00 
 8050e8c:	c7 44 24 28 00 00 00 	movl   $0x0,0x28(%esp)
 8050e93:	00 
 8050e94:	c7 44 24 2c 00 00 00 	movl   $0x0,0x2c(%esp)
 8050e9b:	00 
   return (int) nk_do_syscall(SYS_STAT, a, b); 
 8050e9c:	8d 44 24 20          	lea    0x20(%esp),%eax
 8050ea0:	89 44 24 08          	mov    %eax,0x8(%esp)
 8050ea4:	8d 44 24 10          	lea    0x10(%esp),%eax
 8050ea8:	89 44 24 04          	mov    %eax,0x4(%esp)
 8050eac:	c7 04 24 14 00 00 00 	movl   $0x14,(%esp)
 8050eb3:	e8 e0 db ff ff       	call   804ea98 <nk_do_syscall>
}
 8050eb8:	83 c4 3c             	add    $0x3c,%esp
 8050ebb:	c3                   	ret    

08050ebc <fcntl>:

int fcntl(int fd, int cmd, int arg)
{
 8050ebc:	57                   	push   %edi
 8050ebd:	83 ec 38             	sub    $0x38,%esp
   int a[] = {(uint32_t) fd, (uint32_t) cmd, (uint32_t) arg,0};
 8050ec0:	8b 44 24 40          	mov    0x40(%esp),%eax
 8050ec4:	89 44 24 10          	mov    %eax,0x10(%esp)
 8050ec8:	8b 44 24 44          	mov    0x44(%esp),%eax
 8050ecc:	89 44 24 14          	mov    %eax,0x14(%esp)
 8050ed0:	8b 44 24 48          	mov    0x48(%esp),%eax
 8050ed4:	89 44 24 18          	mov    %eax,0x18(%esp)
 8050ed8:	c7 44 24 1c 00 00 00 	movl   $0x0,0x1c(%esp)
 8050edf:	00 
   int b[] = {0,0,0,0};
 8050ee0:	8d 7c 24 20          	lea    0x20(%esp),%edi
 8050ee4:	b9 10 00 00 00       	mov    $0x10,%ecx
 8050ee9:	31 c0                	xor    %eax,%eax
 8050eeb:	f3 aa                	rep stos %al,%es:(%edi)
   return (int) nk_do_syscall(SYS_FCNTL, a, b);	
 8050eed:	8d 44 24 20          	lea    0x20(%esp),%eax
 8050ef1:	89 44 24 08          	mov    %eax,0x8(%esp)
 8050ef5:	8d 44 24 10          	lea    0x10(%esp),%eax
 8050ef9:	89 44 24 04          	mov    %eax,0x4(%esp)
 8050efd:	c7 04 24 26 00 00 00 	movl   $0x26,(%esp)
 8050f04:	e8 8f db ff ff       	call   804ea98 <nk_do_syscall>
}
 8050f09:	83 c4 38             	add    $0x38,%esp
 8050f0c:	5f                   	pop    %edi
 8050f0d:	c3                   	ret    
 8050f0e:	66 90                	xchg   %ax,%ax

08050f10 <ioctl>:

int ioctl(int fd, int cmd, int arg)
{
 8050f10:	57                   	push   %edi
 8050f11:	83 ec 38             	sub    $0x38,%esp
   int a[] = {(uint32_t) fd, (uint32_t) cmd, (uint32_t) arg,0};
 8050f14:	8b 44 24 40          	mov    0x40(%esp),%eax
 8050f18:	89 44 24 10          	mov    %eax,0x10(%esp)
 8050f1c:	8b 44 24 44          	mov    0x44(%esp),%eax
 8050f20:	89 44 24 14          	mov    %eax,0x14(%esp)
 8050f24:	8b 44 24 48          	mov    0x48(%esp),%eax
 8050f28:	89 44 24 18          	mov    %eax,0x18(%esp)
 8050f2c:	c7 44 24 1c 00 00 00 	movl   $0x0,0x1c(%esp)
 8050f33:	00 
   int b[] = {0,0,0,0};
 8050f34:	8d 7c 24 20          	lea    0x20(%esp),%edi
 8050f38:	b9 10 00 00 00       	mov    $0x10,%ecx
 8050f3d:	31 c0                	xor    %eax,%eax
 8050f3f:	f3 aa                	rep stos %al,%es:(%edi)
   return (int) nk_do_syscall(SYS_IOCTL, a, b);	
 8050f41:	8d 44 24 20          	lea    0x20(%esp),%eax
 8050f45:	89 44 24 08          	mov    %eax,0x8(%esp)
 8050f49:	8d 44 24 10          	lea    0x10(%esp),%eax
 8050f4d:	89 44 24 04          	mov    %eax,0x4(%esp)
 8050f51:	c7 04 24 2c 00 00 00 	movl   $0x2c,(%esp)
 8050f58:	e8 3b db ff ff       	call   804ea98 <nk_do_syscall>
}
 8050f5d:	83 c4 38             	add    $0x38,%esp
 8050f60:	5f                   	pop    %edi
 8050f61:	c3                   	ret    
 8050f62:	66 90                	xchg   %ax,%ax

08050f64 <chdir>:

int chdir(const char *path)
{
 8050f64:	83 ec 3c             	sub    $0x3c,%esp
 8050f67:	8b 44 24 40          	mov    0x40(%esp),%eax
   int a[] = {(uint32_t) path, 0,0,0};
 8050f6b:	89 44 24 10          	mov    %eax,0x10(%esp)
 8050f6f:	c7 44 24 14 00 00 00 	movl   $0x0,0x14(%esp)
 8050f76:	00 
 8050f77:	c7 44 24 18 00 00 00 	movl   $0x0,0x18(%esp)
 8050f7e:	00 
 8050f7f:	c7 44 24 1c 00 00 00 	movl   $0x0,0x1c(%esp)
 8050f86:	00 
   int b[] = {(uint32_t) 1+strlen(path),0,0,0};
 8050f87:	89 04 24             	mov    %eax,(%esp)
 8050f8a:	e8 61 7c ff ff       	call   8048bf0 <strlen>
 8050f8f:	40                   	inc    %eax
 8050f90:	89 44 24 20          	mov    %eax,0x20(%esp)
 8050f94:	c7 44 24 24 00 00 00 	movl   $0x0,0x24(%esp)
 8050f9b:	00 
 8050f9c:	c7 44 24 28 00 00 00 	movl   $0x0,0x28(%esp)
 8050fa3:	00 
 8050fa4:	c7 44 24 2c 00 00 00 	movl   $0x0,0x2c(%esp)
 8050fab:	00 
   return (int) nk_do_syscall(SYS_CHDIR, a, b);
 8050fac:	8d 44 24 20          	lea    0x20(%esp),%eax
 8050fb0:	89 44 24 08          	mov    %eax,0x8(%esp)
 8050fb4:	8d 44 24 10          	lea    0x10(%esp),%eax
 8050fb8:	89 44 24 04          	mov    %eax,0x4(%esp)
 8050fbc:	c7 04 24 0f 00 00 00 	movl   $0xf,(%esp)
 8050fc3:	e8 d0 da ff ff       	call   804ea98 <nk_do_syscall>
}
 8050fc8:	83 c4 3c             	add    $0x3c,%esp
 8050fcb:	c3                   	ret    

08050fcc <chmod>:

int chmod(const char *path, mode_t mode)
{
 8050fcc:	83 ec 3c             	sub    $0x3c,%esp
 8050fcf:	8b 44 24 40          	mov    0x40(%esp),%eax
   int a[] = {(uint32_t) path, (uint32_t) mode,0,0};
 8050fd3:	89 44 24 10          	mov    %eax,0x10(%esp)
 8050fd7:	8b 54 24 44          	mov    0x44(%esp),%edx
 8050fdb:	89 54 24 14          	mov    %edx,0x14(%esp)
 8050fdf:	c7 44 24 18 00 00 00 	movl   $0x0,0x18(%esp)
 8050fe6:	00 
 8050fe7:	c7 44 24 1c 00 00 00 	movl   $0x0,0x1c(%esp)
 8050fee:	00 
   int b[] = {(uint32_t) 1+strlen(path),0,0,0};
 8050fef:	89 04 24             	mov    %eax,(%esp)
 8050ff2:	e8 f9 7b ff ff       	call   8048bf0 <strlen>
 8050ff7:	40                   	inc    %eax
 8050ff8:	89 44 24 20          	mov    %eax,0x20(%esp)
 8050ffc:	c7 44 24 24 00 00 00 	movl   $0x0,0x24(%esp)
 8051003:	00 
 8051004:	c7 44 24 28 00 00 00 	movl   $0x0,0x28(%esp)
 805100b:	00 
 805100c:	c7 44 24 2c 00 00 00 	movl   $0x0,0x2c(%esp)
 8051013:	00 
   return (int) nk_do_syscall(SYS_CHMOD, a, b);
 8051014:	8d 44 24 20          	lea    0x20(%esp),%eax
 8051018:	89 44 24 08          	mov    %eax,0x8(%esp)
 805101c:	8d 44 24 10          	lea    0x10(%esp),%eax
 8051020:	89 44 24 04          	mov    %eax,0x4(%esp)
 8051024:	c7 04 24 11 00 00 00 	movl   $0x11,(%esp)
 805102b:	e8 68 da ff ff       	call   804ea98 <nk_do_syscall>
}
 8051030:	83 c4 3c             	add    $0x3c,%esp
 8051033:	c3                   	ret    

08051034 <mknod>:

int mknod(const char *path, mode_t mode, dev_t dev)
{
 8051034:	83 ec 3c             	sub    $0x3c,%esp
 8051037:	8b 44 24 40          	mov    0x40(%esp),%eax
 805103b:	0f bf 54 24 48       	movswl 0x48(%esp),%edx
   int a[] = {(uint32_t) path, (uint32_t) mode, (uint32_t) dev,0};
 8051040:	89 44 24 10          	mov    %eax,0x10(%esp)
 8051044:	8b 4c 24 44          	mov    0x44(%esp),%ecx
 8051048:	89 4c 24 14          	mov    %ecx,0x14(%esp)
 805104c:	89 54 24 18          	mov    %edx,0x18(%esp)
 8051050:	c7 44 24 1c 00 00 00 	movl   $0x0,0x1c(%esp)
 8051057:	00 
   int b[] = {(uint32_t) 1+strlen(path),0,0,0};
 8051058:	89 04 24             	mov    %eax,(%esp)
 805105b:	e8 90 7b ff ff       	call   8048bf0 <strlen>
 8051060:	40                   	inc    %eax
 8051061:	89 44 24 20          	mov    %eax,0x20(%esp)
 8051065:	c7 44 24 24 00 00 00 	movl   $0x0,0x24(%esp)
 805106c:	00 
 805106d:	c7 44 24 28 00 00 00 	movl   $0x0,0x28(%esp)
 8051074:	00 
 8051075:	c7 44 24 2c 00 00 00 	movl   $0x0,0x2c(%esp)
 805107c:	00 
   return (int) nk_do_syscall(SYS_MKNOD, a, b);
 805107d:	8d 44 24 20          	lea    0x20(%esp),%eax
 8051081:	89 44 24 08          	mov    %eax,0x8(%esp)
 8051085:	8d 44 24 10          	lea    0x10(%esp),%eax
 8051089:	89 44 24 04          	mov    %eax,0x4(%esp)
 805108d:	c7 04 24 12 00 00 00 	movl   $0x12,(%esp)
 8051094:	e8 ff d9 ff ff       	call   804ea98 <nk_do_syscall>
}
 8051099:	83 c4 3c             	add    $0x3c,%esp
 805109c:	c3                   	ret    
 805109d:	8d 76 00             	lea    0x0(%esi),%esi

080510a0 <fchdir>:

int fchdir(int fd)
{
 80510a0:	57                   	push   %edi
 80510a1:	83 ec 38             	sub    $0x38,%esp
   int a[] = {(uint32_t) fd,0,0,0};
 80510a4:	8b 44 24 40          	mov    0x40(%esp),%eax
 80510a8:	89 44 24 10          	mov    %eax,0x10(%esp)
 80510ac:	c7 44 24 14 00 00 00 	movl   $0x0,0x14(%esp)
 80510b3:	00 
 80510b4:	c7 44 24 18 00 00 00 	movl   $0x0,0x18(%esp)
 80510bb:	00 
 80510bc:	c7 44 24 1c 00 00 00 	movl   $0x0,0x1c(%esp)
 80510c3:	00 
   int b[] = {0,0,0,0};
 80510c4:	8d 7c 24 20          	lea    0x20(%esp),%edi
 80510c8:	b9 10 00 00 00       	mov    $0x10,%ecx
 80510cd:	31 c0                	xor    %eax,%eax
 80510cf:	f3 aa                	rep stos %al,%es:(%edi)
   return (int) nk_do_syscall(SYS_FCHDIR, a, b);
 80510d1:	8d 44 24 20          	lea    0x20(%esp),%eax
 80510d5:	89 44 24 08          	mov    %eax,0x8(%esp)
 80510d9:	8d 44 24 10          	lea    0x10(%esp),%eax
 80510dd:	89 44 24 04          	mov    %eax,0x4(%esp)
 80510e1:	c7 04 24 15 00 00 00 	movl   $0x15,(%esp)
 80510e8:	e8 ab d9 ff ff       	call   804ea98 <nk_do_syscall>
}
 80510ed:	83 c4 38             	add    $0x38,%esp
 80510f0:	5f                   	pop    %edi
 80510f1:	c3                   	ret    
 80510f2:	66 90                	xchg   %ax,%ax

080510f4 <umask>:

mode_t umask(mode_t mask)
{
 80510f4:	57                   	push   %edi
 80510f5:	83 ec 38             	sub    $0x38,%esp
   int a[] = {(uint32_t) mask,0,0,0};
 80510f8:	8b 44 24 40          	mov    0x40(%esp),%eax
 80510fc:	89 44 24 10          	mov    %eax,0x10(%esp)
 8051100:	c7 44 24 14 00 00 00 	movl   $0x0,0x14(%esp)
 8051107:	00 
 8051108:	c7 44 24 18 00 00 00 	movl   $0x0,0x18(%esp)
 805110f:	00 
 8051110:	c7 44 24 1c 00 00 00 	movl   $0x0,0x1c(%esp)
 8051117:	00 
   int b[] = {0,0,0,0};
 8051118:	8d 7c 24 20          	lea    0x20(%esp),%edi
 805111c:	b9 10 00 00 00       	mov    $0x10,%ecx
 8051121:	31 c0                	xor    %eax,%eax
 8051123:	f3 aa                	rep stos %al,%es:(%edi)
   return (mode_t) nk_do_syscall(SYS_UMASK, a, b);
 8051125:	8d 44 24 20          	lea    0x20(%esp),%eax
 8051129:	89 44 24 08          	mov    %eax,0x8(%esp)
 805112d:	8d 44 24 10          	lea    0x10(%esp),%eax
 8051131:	89 44 24 04          	mov    %eax,0x4(%esp)
 8051135:	c7 04 24 27 00 00 00 	movl   $0x27,(%esp)
 805113c:	e8 57 d9 ff ff       	call   804ea98 <nk_do_syscall>

}
 8051141:	83 c4 38             	add    $0x38,%esp
 8051144:	5f                   	pop    %edi
 8051145:	c3                   	ret    
 8051146:	66 90                	xchg   %ax,%ax

08051148 <gettimeofday>:
#include <sys/time.h>

int gettimeofday(struct timeval *tv, void *tz)
{
 8051148:	57                   	push   %edi
 8051149:	83 ec 38             	sub    $0x38,%esp
	int a[] = {0,0,0,0};
 805114c:	8d 7c 24 10          	lea    0x10(%esp),%edi
 8051150:	31 c0                	xor    %eax,%eax
 8051152:	b9 10 00 00 00       	mov    $0x10,%ecx
 8051157:	f3 aa                	rep stos %al,%es:(%edi)
	int b[] = {0,0,0,0};
 8051159:	8d 7c 24 20          	lea    0x20(%esp),%edi
 805115d:	b1 10                	mov    $0x10,%cl
 805115f:	f3 aa                	rep stos %al,%es:(%edi)
	
	tv->tv_sec = (time_t) nk_do_syscall(SYS_TIME, a, b);
 8051161:	8d 44 24 20          	lea    0x20(%esp),%eax
 8051165:	89 44 24 08          	mov    %eax,0x8(%esp)
 8051169:	8d 44 24 10          	lea    0x10(%esp),%eax
 805116d:	89 44 24 04          	mov    %eax,0x4(%esp)
 8051171:	c7 04 24 30 00 00 00 	movl   $0x30,(%esp)
 8051178:	e8 1b d9 ff ff       	call   804ea98 <nk_do_syscall>
 805117d:	8b 54 24 40          	mov    0x40(%esp),%edx
 8051181:	89 02                	mov    %eax,(%edx)
	return 0;
}
 8051183:	31 c0                	xor    %eax,%eax
 8051185:	83 c4 38             	add    $0x38,%esp
 8051188:	5f                   	pop    %edi
 8051189:	c3                   	ret    
 805118a:	66 90                	xchg   %ax,%ax

0805118c <access>:

int access(const char *pathname, int mode){
	return 0;
}
 805118c:	31 c0                	xor    %eax,%eax
 805118e:	c3                   	ret    
 805118f:	90                   	nop

08051190 <getdents>:
int getdents (int fd, void *dp, int count)
{
 8051190:	57                   	push   %edi
 8051191:	83 ec 38             	sub    $0x38,%esp
   int a[] = {(uint32_t) fd, (uint32_t) dp, (uint32_t) count,0};
 8051194:	8b 44 24 40          	mov    0x40(%esp),%eax
 8051198:	89 44 24 10          	mov    %eax,0x10(%esp)
 805119c:	8b 44 24 44          	mov    0x44(%esp),%eax
 80511a0:	89 44 24 14          	mov    %eax,0x14(%esp)
 80511a4:	8b 44 24 48          	mov    0x48(%esp),%eax
 80511a8:	89 44 24 18          	mov    %eax,0x18(%esp)
 80511ac:	c7 44 24 1c 00 00 00 	movl   $0x0,0x1c(%esp)
 80511b3:	00 
   int b[] = {0, 0, 0, 0};
 80511b4:	8d 7c 24 20          	lea    0x20(%esp),%edi
 80511b8:	b9 10 00 00 00       	mov    $0x10,%ecx
 80511bd:	31 c0                	xor    %eax,%eax
 80511bf:	f3 aa                	rep stos %al,%es:(%edi)
   return (int) nk_do_syscall(SYS_GETDENTS, a, b); 
 80511c1:	8d 44 24 20          	lea    0x20(%esp),%eax
 80511c5:	89 44 24 08          	mov    %eax,0x8(%esp)
 80511c9:	8d 44 24 10          	lea    0x10(%esp),%eax
 80511cd:	89 44 24 04          	mov    %eax,0x4(%esp)
 80511d1:	c7 04 24 2b 00 00 00 	movl   $0x2b,(%esp)
 80511d8:	e8 bb d8 ff ff       	call   804ea98 <nk_do_syscall>
}
 80511dd:	83 c4 38             	add    $0x38,%esp
 80511e0:	5f                   	pop    %edi
 80511e1:	c3                   	ret    
 80511e2:	66 90                	xchg   %ax,%ax

080511e4 <lstat>:

int
_DEFUN (lstat, (file, st),
        const char  *file _AND
        struct stat *st)
{
 80511e4:	83 ec 3c             	sub    $0x3c,%esp
 80511e7:	8b 44 24 40          	mov    0x40(%esp),%eax
   int a[] = {(uint32_t) file, (uint32_t) st,0,0};
 80511eb:	89 44 24 10          	mov    %eax,0x10(%esp)
 80511ef:	8b 54 24 44          	mov    0x44(%esp),%edx
 80511f3:	89 54 24 14          	mov    %edx,0x14(%esp)
 80511f7:	c7 44 24 18 00 00 00 	movl   $0x0,0x18(%esp)
 80511fe:	00 
 80511ff:	c7 44 24 1c 00 00 00 	movl   $0x0,0x1c(%esp)
 8051206:	00 
   int b[] = {(uint32_t) 1+strlen(file),0,0,0};
 8051207:	89 04 24             	mov    %eax,(%esp)
 805120a:	e8 e1 79 ff ff       	call   8048bf0 <strlen>
 805120f:	40                   	inc    %eax
 8051210:	89 44 24 20          	mov    %eax,0x20(%esp)
 8051214:	c7 44 24 24 00 00 00 	movl   $0x0,0x24(%esp)
 805121b:	00 
 805121c:	c7 44 24 28 00 00 00 	movl   $0x0,0x28(%esp)
 8051223:	00 
 8051224:	c7 44 24 2c 00 00 00 	movl   $0x0,0x2c(%esp)
 805122b:	00 
   return (int) nk_do_syscall(SYS_LSTAT, a, b); 
 805122c:	8d 44 24 20          	lea    0x20(%esp),%eax
 8051230:	89 44 24 08          	mov    %eax,0x8(%esp)
 8051234:	8d 44 24 10          	lea    0x10(%esp),%eax
 8051238:	89 44 24 04          	mov    %eax,0x4(%esp)
 805123c:	c7 04 24 29 00 00 00 	movl   $0x29,(%esp)
 8051243:	e8 50 d8 ff ff       	call   804ea98 <nk_do_syscall>
}
 8051248:	83 c4 3c             	add    $0x3c,%esp
 805124b:	c3                   	ret    

0805124c <symlink>:

int
_DEFUN (symlink, (path1, path2),
        const char *path1 _AND
        const char *path2)
{
 805124c:	53                   	push   %ebx
 805124d:	83 ec 38             	sub    $0x38,%esp
 8051250:	8b 44 24 40          	mov    0x40(%esp),%eax
 8051254:	8b 5c 24 44          	mov    0x44(%esp),%ebx
   int a[] = {(uint32_t) path1, (uint32_t) path2, 0,0};
 8051258:	89 44 24 10          	mov    %eax,0x10(%esp)
 805125c:	89 5c 24 14          	mov    %ebx,0x14(%esp)
 8051260:	c7 44 24 18 00 00 00 	movl   $0x0,0x18(%esp)
 8051267:	00 
 8051268:	c7 44 24 1c 00 00 00 	movl   $0x0,0x1c(%esp)
 805126f:	00 
   int b[] = {(uint32_t) 1+strlen(path1), 1+strlen(path2),0,0};
 8051270:	89 04 24             	mov    %eax,(%esp)
 8051273:	e8 78 79 ff ff       	call   8048bf0 <strlen>
 8051278:	40                   	inc    %eax
 8051279:	89 44 24 20          	mov    %eax,0x20(%esp)
 805127d:	89 1c 24             	mov    %ebx,(%esp)
 8051280:	e8 6b 79 ff ff       	call   8048bf0 <strlen>
 8051285:	40                   	inc    %eax
 8051286:	89 44 24 24          	mov    %eax,0x24(%esp)
 805128a:	c7 44 24 28 00 00 00 	movl   $0x0,0x28(%esp)
 8051291:	00 
 8051292:	c7 44 24 2c 00 00 00 	movl   $0x0,0x2c(%esp)
 8051299:	00 
   return (int) nk_do_syscall(SYS_SYMLINK, a, b); 
 805129a:	8d 44 24 20          	lea    0x20(%esp),%eax
 805129e:	89 44 24 08          	mov    %eax,0x8(%esp)
 80512a2:	8d 44 24 10          	lea    0x10(%esp),%eax
 80512a6:	89 44 24 04          	mov    %eax,0x4(%esp)
 80512aa:	c7 04 24 2a 00 00 00 	movl   $0x2a,(%esp)
 80512b1:	e8 e2 d7 ff ff       	call   804ea98 <nk_do_syscall>
}
 80512b6:	83 c4 38             	add    $0x38,%esp
 80512b9:	5b                   	pop    %ebx
 80512ba:	c3                   	ret    
 80512bb:	90                   	nop

080512bc <unlink>:

int
_DEFUN (unlink, (name),
        char *name)
{
 80512bc:	83 ec 3c             	sub    $0x3c,%esp
 80512bf:	8b 44 24 40          	mov    0x40(%esp),%eax
   int a[] = {(uint32_t) name, 0,0,0};
 80512c3:	89 44 24 10          	mov    %eax,0x10(%esp)
 80512c7:	c7 44 24 14 00 00 00 	movl   $0x0,0x14(%esp)
 80512ce:	00 
 80512cf:	c7 44 24 18 00 00 00 	movl   $0x0,0x18(%esp)
 80512d6:	00 
 80512d7:	c7 44 24 1c 00 00 00 	movl   $0x0,0x1c(%esp)
 80512de:	00 
   int b[] = {(uint32_t) 1+strlen(name),0,0,0};
 80512df:	89 04 24             	mov    %eax,(%esp)
 80512e2:	e8 09 79 ff ff       	call   8048bf0 <strlen>
 80512e7:	40                   	inc    %eax
 80512e8:	89 44 24 20          	mov    %eax,0x20(%esp)
 80512ec:	c7 44 24 24 00 00 00 	movl   $0x0,0x24(%esp)
 80512f3:	00 
 80512f4:	c7 44 24 28 00 00 00 	movl   $0x0,0x28(%esp)
 80512fb:	00 
 80512fc:	c7 44 24 2c 00 00 00 	movl   $0x0,0x2c(%esp)
 8051303:	00 
   return (int) nk_do_syscall(SYS_UNLINK, a, b);
 8051304:	8d 44 24 20          	lea    0x20(%esp),%eax
 8051308:	89 44 24 08          	mov    %eax,0x8(%esp)
 805130c:	8d 44 24 10          	lea    0x10(%esp),%eax
 8051310:	89 44 24 04          	mov    %eax,0x4(%esp)
 8051314:	c7 04 24 0e 00 00 00 	movl   $0xe,(%esp)
 805131b:	e8 78 d7 ff ff       	call   804ea98 <nk_do_syscall>
}
 8051320:	83 c4 3c             	add    $0x3c,%esp
 8051323:	c3                   	ret    

08051324 <write>:
int
_DEFUN (write, (file, ptr, len),
        int   file  _AND
        char *ptr   _AND
        int   len)
{
 8051324:	57                   	push   %edi
 8051325:	83 ec 38             	sub    $0x38,%esp
   int a[] = {(uint32_t) file, (uint32_t) ptr, (uint32_t) len,0};
 8051328:	8b 44 24 40          	mov    0x40(%esp),%eax
 805132c:	89 44 24 10          	mov    %eax,0x10(%esp)
 8051330:	8b 44 24 44          	mov    0x44(%esp),%eax
 8051334:	89 44 24 14          	mov    %eax,0x14(%esp)
 8051338:	8b 44 24 48          	mov    0x48(%esp),%eax
 805133c:	89 44 24 18          	mov    %eax,0x18(%esp)
 8051340:	c7 44 24 1c 00 00 00 	movl   $0x0,0x1c(%esp)
 8051347:	00 
   int b[] = {0, 0, 0, 0};
 8051348:	8d 7c 24 20          	lea    0x20(%esp),%edi
 805134c:	b9 10 00 00 00       	mov    $0x10,%ecx
 8051351:	31 c0                	xor    %eax,%eax
 8051353:	f3 aa                	rep stos %al,%es:(%edi)
   return (int) nk_do_syscall(SYS_WRITE, a, b); 
 8051355:	8d 44 24 20          	lea    0x20(%esp),%eax
 8051359:	89 44 24 08          	mov    %eax,0x8(%esp)
 805135d:	8d 44 24 10          	lea    0x10(%esp),%eax
 8051361:	89 44 24 04          	mov    %eax,0x4(%esp)
 8051365:	c7 04 24 20 00 00 00 	movl   $0x20,(%esp)
 805136c:	e8 27 d7 ff ff       	call   804ea98 <nk_do_syscall>
}
 8051371:	83 c4 38             	add    $0x38,%esp
 8051374:	5f                   	pop    %edi
 8051375:	c3                   	ret    
 8051376:	90                   	nop
 8051377:	90                   	nop

08051378 <__swbuf_r>:
int
_DEFUN(__swbuf_r, (ptr, c, fp),
       struct _reent *ptr _AND
       register int c _AND
       register FILE *fp)
{
 8051378:	57                   	push   %edi
 8051379:	56                   	push   %esi
 805137a:	53                   	push   %ebx
 805137b:	83 ec 10             	sub    $0x10,%esp
 805137e:	8b 74 24 20          	mov    0x20(%esp),%esi
 8051382:	8b 5c 24 28          	mov    0x28(%esp),%ebx
  register int n;

  /* Ensure stdio has been initialized.  */

  CHECK_INIT (ptr, fp);
 8051386:	85 f6                	test   %esi,%esi
 8051388:	74 0b                	je     8051395 <__swbuf_r+0x1d>
 805138a:	8b 46 38             	mov    0x38(%esi),%eax
 805138d:	85 c0                	test   %eax,%eax
 805138f:	0f 84 cf 00 00 00    	je     8051464 <__swbuf_r+0xec>
   * (if line buffered) so that we will get called again.
   * If we did not do this, a sufficient number of putc()
   * calls might wrap _w from negative to positive.
   */

  fp->_w = fp->_lbfsize;
 8051395:	8b 43 18             	mov    0x18(%ebx),%eax
 8051398:	89 43 08             	mov    %eax,0x8(%ebx)
  if (cantwrite (ptr, fp))
 805139b:	8b 53 0c             	mov    0xc(%ebx),%edx
 805139e:	f6 c2 08             	test   $0x8,%dl
 80513a1:	0f 84 a1 00 00 00    	je     8051448 <__swbuf_r+0xd0>
 80513a7:	8b 43 10             	mov    0x10(%ebx),%eax
 80513aa:	85 c0                	test   %eax,%eax
 80513ac:	0f 84 96 00 00 00    	je     8051448 <__swbuf_r+0xd0>
    return EOF;
  c = (unsigned char) c;
 80513b2:	0f b6 7c 24 24       	movzbl 0x24(%esp),%edi

  ORIENT (fp, -1);
 80513b7:	f6 c6 20             	test   $0x20,%dh
 80513ba:	74 34                	je     80513f0 <__swbuf_r+0x78>
   * happen on unbuffered streams, where _bf._size==1; fflush()
   * guarantees that putc() will always call wbuf() by setting _w
   * to 0, so we need not do anything else.
   */

  n = fp->_p - fp->_bf._base;
 80513bc:	8b 13                	mov    (%ebx),%edx
 80513be:	89 d1                	mov    %edx,%ecx
 80513c0:	29 c1                	sub    %eax,%ecx
 80513c2:	89 c8                	mov    %ecx,%eax
  if (n >= fp->_bf._size)
 80513c4:	3b 4b 14             	cmp    0x14(%ebx),%ecx
 80513c7:	7d 42                	jge    805140b <__swbuf_r+0x93>
 80513c9:	40                   	inc    %eax
    {
      if (_fflush_r (ptr, fp))
	return EOF;
      n = 0;
    }
  fp->_w--;
 80513ca:	ff 4b 08             	decl   0x8(%ebx)
  *fp->_p++ = c;
 80513cd:	89 f9                	mov    %edi,%ecx
 80513cf:	88 0a                	mov    %cl,(%edx)
 80513d1:	42                   	inc    %edx
 80513d2:	89 13                	mov    %edx,(%ebx)
  if (++n == fp->_bf._size || (fp->_flags & __SLBF && c == '\n'))
 80513d4:	39 43 14             	cmp    %eax,0x14(%ebx)
 80513d7:	74 57                	je     8051430 <__swbuf_r+0xb8>
 80513d9:	f6 43 0c 01          	testb  $0x1,0xc(%ebx)
 80513dd:	74 05                	je     80513e4 <__swbuf_r+0x6c>
 80513df:	83 ff 0a             	cmp    $0xa,%edi
 80513e2:	74 4c                	je     8051430 <__swbuf_r+0xb8>
    if (_fflush_r (ptr, fp))
      return EOF;
  return c;
}
 80513e4:	89 f8                	mov    %edi,%eax
 80513e6:	83 c4 10             	add    $0x10,%esp
 80513e9:	5b                   	pop    %ebx
 80513ea:	5e                   	pop    %esi
 80513eb:	5f                   	pop    %edi
 80513ec:	c3                   	ret    
 80513ed:	8d 76 00             	lea    0x0(%esi),%esi
  fp->_w = fp->_lbfsize;
  if (cantwrite (ptr, fp))
    return EOF;
  c = (unsigned char) c;

  ORIENT (fp, -1);
 80513f0:	80 ce 20             	or     $0x20,%dh
 80513f3:	66 89 53 0c          	mov    %dx,0xc(%ebx)
 80513f7:	81 63 64 ff df ff ff 	andl   $0xffffdfff,0x64(%ebx)
   * happen on unbuffered streams, where _bf._size==1; fflush()
   * guarantees that putc() will always call wbuf() by setting _w
   * to 0, so we need not do anything else.
   */

  n = fp->_p - fp->_bf._base;
 80513fe:	8b 13                	mov    (%ebx),%edx
 8051400:	89 d1                	mov    %edx,%ecx
 8051402:	29 c1                	sub    %eax,%ecx
 8051404:	89 c8                	mov    %ecx,%eax
  if (n >= fp->_bf._size)
 8051406:	3b 4b 14             	cmp    0x14(%ebx),%ecx
 8051409:	7c be                	jl     80513c9 <__swbuf_r+0x51>
    {
      if (_fflush_r (ptr, fp))
 805140b:	89 5c 24 04          	mov    %ebx,0x4(%esp)
 805140f:	89 34 24             	mov    %esi,(%esp)
 8051412:	e8 a9 b2 ff ff       	call   804c6c0 <_fflush_r>
 8051417:	85 c0                	test   %eax,%eax
 8051419:	75 25                	jne    8051440 <__swbuf_r+0xc8>
 805141b:	8b 13                	mov    (%ebx),%edx
 805141d:	b0 01                	mov    $0x1,%al
	return EOF;
      n = 0;
    }
  fp->_w--;
 805141f:	ff 4b 08             	decl   0x8(%ebx)
  *fp->_p++ = c;
 8051422:	89 f9                	mov    %edi,%ecx
 8051424:	88 0a                	mov    %cl,(%edx)
 8051426:	42                   	inc    %edx
 8051427:	89 13                	mov    %edx,(%ebx)
  if (++n == fp->_bf._size || (fp->_flags & __SLBF && c == '\n'))
 8051429:	39 43 14             	cmp    %eax,0x14(%ebx)
 805142c:	75 ab                	jne    80513d9 <__swbuf_r+0x61>
 805142e:	66 90                	xchg   %ax,%ax
    if (_fflush_r (ptr, fp))
 8051430:	89 5c 24 04          	mov    %ebx,0x4(%esp)
 8051434:	89 34 24             	mov    %esi,(%esp)
 8051437:	e8 84 b2 ff ff       	call   804c6c0 <_fflush_r>
 805143c:	85 c0                	test   %eax,%eax
 805143e:	74 a4                	je     80513e4 <__swbuf_r+0x6c>
   * calls might wrap _w from negative to positive.
   */

  fp->_w = fp->_lbfsize;
  if (cantwrite (ptr, fp))
    return EOF;
 8051440:	bf ff ff ff ff       	mov    $0xffffffff,%edi
 8051445:	eb 9d                	jmp    80513e4 <__swbuf_r+0x6c>
 8051447:	90                   	nop
   * If we did not do this, a sufficient number of putc()
   * calls might wrap _w from negative to positive.
   */

  fp->_w = fp->_lbfsize;
  if (cantwrite (ptr, fp))
 8051448:	89 5c 24 04          	mov    %ebx,0x4(%esp)
 805144c:	89 34 24             	mov    %esi,(%esp)
 805144f:	e8 64 99 ff ff       	call   804adb8 <__swsetup_r>
 8051454:	85 c0                	test   %eax,%eax
 8051456:	75 e8                	jne    8051440 <__swbuf_r+0xc8>
 8051458:	8b 53 0c             	mov    0xc(%ebx),%edx
 805145b:	8b 43 10             	mov    0x10(%ebx),%eax
 805145e:	e9 4f ff ff ff       	jmp    80513b2 <__swbuf_r+0x3a>
 8051463:	90                   	nop
{
  register int n;

  /* Ensure stdio has been initialized.  */

  CHECK_INIT (ptr, fp);
 8051464:	89 34 24             	mov    %esi,(%esp)
 8051467:	e8 f8 b3 ff ff       	call   804c864 <__sinit>
 805146c:	e9 24 ff ff ff       	jmp    8051395 <__swbuf_r+0x1d>
 8051471:	8d 76 00             	lea    0x0(%esi),%esi

08051474 <__swbuf>:
   earlier dynamically built newlib libraries. */
int
_DEFUN(__swbuf, (c, fp),
       register int c _AND
       register FILE *fp)
{
 8051474:	83 ec 1c             	sub    $0x1c,%esp
  return __swbuf_r (_REENT, c, fp);
 8051477:	8b 44 24 24          	mov    0x24(%esp),%eax
 805147b:	89 44 24 08          	mov    %eax,0x8(%esp)
 805147f:	8b 44 24 20          	mov    0x20(%esp),%eax
 8051483:	89 44 24 04          	mov    %eax,0x4(%esp)
 8051487:	a1 40 40 05 08       	mov    0x8054040,%eax
 805148c:	89 04 24             	mov    %eax,(%esp)
 805148f:	e8 e4 fe ff ff       	call   8051378 <__swbuf_r>
}
 8051494:	83 c4 1c             	add    $0x1c,%esp
 8051497:	c3                   	ret    

08051498 <_wcrtomb_r>:
_DEFUN (_wcrtomb_r, (ptr, s, wc, ps),
	struct _reent *ptr _AND
	char *s _AND
	wchar_t wc _AND
	mbstate_t *ps)
{
 8051498:	55                   	push   %ebp
 8051499:	57                   	push   %edi
 805149a:	56                   	push   %esi
 805149b:	53                   	push   %ebx
 805149c:	83 ec 3c             	sub    $0x3c,%esp
 805149f:	8b 74 24 50          	mov    0x50(%esp),%esi
 80514a3:	8b 5c 24 54          	mov    0x54(%esp),%ebx
 80514a7:	8b 7c 24 5c          	mov    0x5c(%esp),%edi
      _REENT_CHECK_MISC(ptr);
      ps = &(_REENT_WCRTOMB_STATE(ptr));
    }
#endif

  if (s == NULL)
 80514ab:	85 db                	test   %ebx,%ebx
 80514ad:	74 3d                	je     80514ec <_wcrtomb_r+0x54>
    retval = __wctomb (ptr, buf, L'\0', __locale_charset (), ps);
  else
    retval = __wctomb (ptr, s, wc, __locale_charset (), ps);
 80514af:	8b 2d 68 49 05 08    	mov    0x8054968,%ebp
 80514b5:	e8 56 be ff ff       	call   804d310 <__locale_charset>
 80514ba:	89 7c 24 10          	mov    %edi,0x10(%esp)
 80514be:	89 44 24 0c          	mov    %eax,0xc(%esp)
 80514c2:	8b 44 24 58          	mov    0x58(%esp),%eax
 80514c6:	89 44 24 08          	mov    %eax,0x8(%esp)
 80514ca:	89 5c 24 04          	mov    %ebx,0x4(%esp)
 80514ce:	89 34 24             	mov    %esi,(%esp)
 80514d1:	ff d5                	call   *%ebp

  if (retval == -1)
 80514d3:	83 f8 ff             	cmp    $0xffffffff,%eax
 80514d6:	75 0c                	jne    80514e4 <_wcrtomb_r+0x4c>
    {
      ps->__count = 0;
 80514d8:	c7 07 00 00 00 00    	movl   $0x0,(%edi)
      ptr->_errno = EILSEQ;
 80514de:	c7 06 8a 00 00 00    	movl   $0x8a,(%esi)
      return (size_t)(-1);
    }
  else
    return (size_t)retval;
}
 80514e4:	83 c4 3c             	add    $0x3c,%esp
 80514e7:	5b                   	pop    %ebx
 80514e8:	5e                   	pop    %esi
 80514e9:	5f                   	pop    %edi
 80514ea:	5d                   	pop    %ebp
 80514eb:	c3                   	ret    
      ps = &(_REENT_WCRTOMB_STATE(ptr));
    }
#endif

  if (s == NULL)
    retval = __wctomb (ptr, buf, L'\0', __locale_charset (), ps);
 80514ec:	8b 1d 68 49 05 08    	mov    0x8054968,%ebx
 80514f2:	e8 19 be ff ff       	call   804d310 <__locale_charset>
 80514f7:	89 7c 24 10          	mov    %edi,0x10(%esp)
 80514fb:	89 44 24 0c          	mov    %eax,0xc(%esp)
 80514ff:	c7 44 24 08 00 00 00 	movl   $0x0,0x8(%esp)
 8051506:	00 
 8051507:	8d 44 24 26          	lea    0x26(%esp),%eax
 805150b:	89 44 24 04          	mov    %eax,0x4(%esp)
 805150f:	89 34 24             	mov    %esi,(%esp)
 8051512:	ff d3                	call   *%ebx
 8051514:	eb bd                	jmp    80514d3 <_wcrtomb_r+0x3b>
 8051516:	66 90                	xchg   %ax,%ax

08051518 <wcrtomb>:
size_t
_DEFUN (wcrtomb, (s, wc, ps),
	char *__restrict s _AND
	wchar_t wc _AND
	mbstate_t *__restrict ps)
{
 8051518:	55                   	push   %ebp
 8051519:	57                   	push   %edi
 805151a:	56                   	push   %esi
 805151b:	53                   	push   %ebx
 805151c:	83 ec 3c             	sub    $0x3c,%esp
 805151f:	8b 5c 24 50          	mov    0x50(%esp),%ebx
 8051523:	8b 7c 24 58          	mov    0x58(%esp),%edi
#if defined(PREFER_SIZE_OVER_SPEED) || defined(__OPTIMIZE_SIZE__)
  return _wcrtomb_r (_REENT, s, wc, ps);
#else
  int retval = 0;
  struct _reent *reent = _REENT;
 8051527:	8b 35 40 40 05 08    	mov    0x8054040,%esi
      _REENT_CHECK_MISC(reent);
      ps = &(_REENT_WCRTOMB_STATE(reent));
    }
#endif

  if (s == NULL)
 805152d:	85 db                	test   %ebx,%ebx
 805152f:	74 3f                	je     8051570 <wcrtomb+0x58>
    retval = __wctomb (reent, buf, L'\0', __locale_charset (), ps);
  else
    retval = __wctomb (reent, s, wc, __locale_charset (), ps);
 8051531:	8b 2d 68 49 05 08    	mov    0x8054968,%ebp
 8051537:	e8 d4 bd ff ff       	call   804d310 <__locale_charset>
 805153c:	89 7c 24 10          	mov    %edi,0x10(%esp)
 8051540:	89 44 24 0c          	mov    %eax,0xc(%esp)
 8051544:	8b 44 24 54          	mov    0x54(%esp),%eax
 8051548:	89 44 24 08          	mov    %eax,0x8(%esp)
 805154c:	89 5c 24 04          	mov    %ebx,0x4(%esp)
 8051550:	89 34 24             	mov    %esi,(%esp)
 8051553:	ff d5                	call   *%ebp

  if (retval == -1)
 8051555:	83 f8 ff             	cmp    $0xffffffff,%eax
 8051558:	75 0c                	jne    8051566 <wcrtomb+0x4e>
    {
      ps->__count = 0;
 805155a:	c7 07 00 00 00 00    	movl   $0x0,(%edi)
      reent->_errno = EILSEQ;
 8051560:	c7 06 8a 00 00 00    	movl   $0x8a,(%esi)
      return (size_t)(-1);
    }
  else
    return (size_t)retval;
#endif /* not PREFER_SIZE_OVER_SPEED */
}
 8051566:	83 c4 3c             	add    $0x3c,%esp
 8051569:	5b                   	pop    %ebx
 805156a:	5e                   	pop    %esi
 805156b:	5f                   	pop    %edi
 805156c:	5d                   	pop    %ebp
 805156d:	c3                   	ret    
 805156e:	66 90                	xchg   %ax,%ax
      ps = &(_REENT_WCRTOMB_STATE(reent));
    }
#endif

  if (s == NULL)
    retval = __wctomb (reent, buf, L'\0', __locale_charset (), ps);
 8051570:	8b 1d 68 49 05 08    	mov    0x8054968,%ebx
 8051576:	e8 95 bd ff ff       	call   804d310 <__locale_charset>
 805157b:	89 7c 24 10          	mov    %edi,0x10(%esp)
 805157f:	89 44 24 0c          	mov    %eax,0xc(%esp)
 8051583:	c7 44 24 08 00 00 00 	movl   $0x0,0x8(%esp)
 805158a:	00 
 805158b:	8d 44 24 26          	lea    0x26(%esp),%eax
 805158f:	89 44 24 04          	mov    %eax,0x4(%esp)
 8051593:	89 34 24             	mov    %esi,(%esp)
 8051596:	ff d3                	call   *%ebx
 8051598:	eb bb                	jmp    8051555 <wcrtomb+0x3d>
 805159a:	90                   	nop
 805159b:	90                   	nop

0805159c <__ascii_wctomb>:
        struct _reent *r       _AND 
        char          *s       _AND
        wchar_t        _wchar  _AND
	const char    *charset _AND
        mbstate_t     *state)
{
 805159c:	8b 44 24 08          	mov    0x8(%esp),%eax
 80515a0:	8b 54 24 0c          	mov    0xc(%esp),%edx
  /* Avoids compiler warnings about comparisons that are always false
     due to limited range when sizeof(wchar_t) is 2 but sizeof(wint_t)
     is 4, as is the case on cygwin.  */
  wint_t wchar = _wchar;

  if (s == NULL)
 80515a4:	85 c0                	test   %eax,%eax
 80515a6:	74 10                	je     80515b8 <__ascii_wctomb+0x1c>
    return 0;
 
#ifdef __CYGWIN__
  if ((size_t)wchar >= 0x80)
#else
  if ((size_t)wchar >= 0x100)
 80515a8:	81 fa ff 00 00 00    	cmp    $0xff,%edx
 80515ae:	77 0c                	ja     80515bc <__ascii_wctomb+0x20>
    {
      r->_errno = EILSEQ;
      return -1;
    }

  *s = (char) wchar;
 80515b0:	88 10                	mov    %dl,(%eax)
  return 1;
 80515b2:	b8 01 00 00 00       	mov    $0x1,%eax
 80515b7:	c3                   	ret    
     due to limited range when sizeof(wchar_t) is 2 but sizeof(wint_t)
     is 4, as is the case on cygwin.  */
  wint_t wchar = _wchar;

  if (s == NULL)
    return 0;
 80515b8:	31 c0                	xor    %eax,%eax
      return -1;
    }

  *s = (char) wchar;
  return 1;
}
 80515ba:	c3                   	ret    
 80515bb:	90                   	nop
  if ((size_t)wchar >= 0x80)
#else
  if ((size_t)wchar >= 0x100)
#endif
    {
      r->_errno = EILSEQ;
 80515bc:	8b 44 24 04          	mov    0x4(%esp),%eax
 80515c0:	c7 00 8a 00 00 00    	movl   $0x8a,(%eax)
      return -1;
 80515c6:	b8 ff ff ff ff       	mov    $0xffffffff,%eax
 80515cb:	c3                   	ret    

080515cc <_wctomb_r>:
_DEFUN (_wctomb_r, (r, s, wchar, state),
        struct _reent *r     _AND 
        char          *s     _AND
        wchar_t        _wchar _AND
        mbstate_t     *state)
{
 80515cc:	53                   	push   %ebx
 80515cd:	83 ec 28             	sub    $0x28,%esp
  return __wctomb (r, s, _wchar, __locale_charset (), state);
 80515d0:	8b 1d 68 49 05 08    	mov    0x8054968,%ebx
 80515d6:	e8 35 bd ff ff       	call   804d310 <__locale_charset>
 80515db:	8b 54 24 3c          	mov    0x3c(%esp),%edx
 80515df:	89 54 24 10          	mov    %edx,0x10(%esp)
 80515e3:	89 44 24 0c          	mov    %eax,0xc(%esp)
 80515e7:	8b 44 24 38          	mov    0x38(%esp),%eax
 80515eb:	89 44 24 08          	mov    %eax,0x8(%esp)
 80515ef:	8b 44 24 34          	mov    0x34(%esp),%eax
 80515f3:	89 44 24 04          	mov    %eax,0x4(%esp)
 80515f7:	8b 44 24 30          	mov    0x30(%esp),%eax
 80515fb:	89 04 24             	mov    %eax,(%esp)
 80515fe:	ff d3                	call   *%ebx
}
 8051600:	83 c4 28             	add    $0x28,%esp
 8051603:	5b                   	pop    %ebx
 8051604:	c3                   	ret    
 8051605:	90                   	nop
 8051606:	90                   	nop
 8051607:	90                   	nop

08051608 <isatty>:
#include <reent.h>

int
_DEFUN(isatty, (fd), int fd)
{
  return _isatty (fd);
 8051608:	e9 ff f5 ff ff       	jmp    8050c0c <_isatty>
 805160d:	90                   	nop
 805160e:	90                   	nop
 805160f:	90                   	nop

08051610 <__do_global_ctors_aux>:
 8051610:	55                   	push   %ebp
 8051611:	89 e5                	mov    %esp,%ebp
 8051613:	53                   	push   %ebx
 8051614:	52                   	push   %edx
 8051615:	a1 00 40 05 08       	mov    0x8054000,%eax
 805161a:	bb 00 40 05 08       	mov    $0x8054000,%ebx
 805161f:	83 f8 ff             	cmp    $0xffffffff,%eax
 8051622:	74 0c                	je     8051630 <__do_global_ctors_aux+0x20>
 8051624:	ff d0                	call   *%eax
 8051626:	83 eb 04             	sub    $0x4,%ebx
 8051629:	8b 03                	mov    (%ebx),%eax
 805162b:	83 f8 ff             	cmp    $0xffffffff,%eax
 805162e:	75 f4                	jne    8051624 <__do_global_ctors_aux+0x14>
 8051630:	58                   	pop    %eax
 8051631:	5b                   	pop    %ebx
 8051632:	5d                   	pop    %ebp
 8051633:	c3                   	ret    

Disassembly of section .fini:

08051634 <.fini>:
 8051634:	e8 cb 6a ff ff       	call   8048104 <__do_global_dtors_aux>
