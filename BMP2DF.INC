const	err:byte=0;
	use0:boolean=true;
	use32:boolean=false;
	lfd_use_0:boolean=true;

procedure fwrite(var f:file;var buf;size:word);
var a:word;
begin
 blockwrite(f,buf,size,a);
 if a<>size then err:=1;
end;

Function Errorcode:integer;
begin
 if err=0 then errorcode:=ioresult
 else begin errorcode:=err; err:=0; end;
end;

procedure fread(var f:file;var buf;size:word);
var a:word;
begin
 blockread(f,buf,size,a);
 if a<>size then err:=1;
end;


function getname(var f:file):fname;
var s:fname;
begin
move(fstr(f).name,s[1],80);
s[0]:=#80;
s[0]:=chr(pos(#0,s)-1);
getname:=s;
end;

procedure Fileerror(s:string;var f:file);
begin
 Writeln('Error: ',s,' file: ',getname(f));
end;

procedure fdelete(var f:file);
begin
 close(f); if ioresult=0 then;
 Writeln('Deleting ',getname(f));
 erase(f);
end;


Function enoughmem(w,h:word;var f:file):integer;
var s:string[20];
begin
enoughmem:=0;
if longint(w)*longint(h)>memavail then
begin
 str(longint(w)*longint(h) div 1024,s);
 Fileerror('Low memory: '+s+' Kb needed',f);
 enoughmem:=-1;
end;
end;


function pa(p:pointer):longint;assembler; {pointer to absolute address}
asm
	les si,p
	mov ax,es
	mov bx,16
	mul bx
	add ax,si
	adc dx,0
end;

function ap(l:longint):pointer; {absolute address to pointer}
begin
	ap:=ptr(l div 16,l mod 16);
end;

function ip(p:pointer;offs:longint):pointer;
begin
	ip:=ap(pa(p)+offs);
end;


procedure readfile(var f:file;buf:pointer;asize:longint);
var i:integer;a,nreads:word;
begin
 nreads:=asize div $8000;
 for i:=1 to  nreads do
		begin
		  fread(f,buf^,$8000);
		  buf:=ptr(seg(buf^)+$800,0);
		end;
 fread(f,buf^,asize mod $8000);
end;

procedure writefile(var f:file; buf:pointer; asize:longint);
var i:integer;a,nwrites:word;
begin
 nwrites:=asize div $8000;
 for i:=1 to  nwrites do
		begin
		  fwrite(f,buf^,$8000);
		  buf:=ptr(seg(buf^)+$800,0);
		end;
 fwrite(f,buf^,asize mod $8000);
end;

Procedure copyfile(var f,f1:file;size:longint;buf:pointer;bufsize:longint);
var i:integer;a,nwrites:word;
begin
nwrites:=size div bufsize;
for i:=1 to nwrites do
		    begin
		     readfile(f,buf,bufsize);
		     writefile(f1,buf,bufsize);
		    end;
 readfile(f,buf,size mod bufsize);
 writefile(f1,buf,size mod bufsize);
end;


procedure getcolumn(b:tbitmap; n:word; var buf);
var p:pointer; w:word;
    h16,r16:word; offs:word;
begin
p:=b.bits;
w:=b.w;
offs:=ofs(b.bits^);
 h16:=b.h div 16;
 r16:=b.h mod 16;
  asm
	push ds
	lds si,p
	les di,buf
	mov bx,n
	mov dx,h16
	cld
	or dx,dx
	jz @rest
  @cyc:
	mov cx,16
  @16:
	mov al,[bx+si]
	stosb
	add si,w
	loop @16
	mov ax,ds
	add ax,w
	mov ds,ax
	mov si,offs
	dec dx
	jnz @cyc
  @rest:
	mov cx,r16
	or cx,cx
	jz @done
  @cyc1:
	mov al,[bx+si]
	stosb
	add si,w
	loop @cyc1
  @done:
	pop ds
  end;
{  for j:=0 to h-1 do plane[j]:=byte(ip(b,j*w+i)^);}
end;

function flipwrite(var f:file; var b:tbitmap):integer;
var i:integer;
begin
 for i:=0 to b.w-1 do
  begin
   getcolumn(b,i,buf);
   fwrite(f,buf,b.h);
  end;
end;

function lg2(value:word):word;
var lg:byte;
 begin
 lg:=0;
 while value<>0 do
  begin
   value:=value div 2;
   inc(lg);
  end;
  dec(lg);
  lg2:=lg;
 end;

Function loadBMP(var f:file; var b:tbitmap):integer; {bottom-up bitmap}
var
    bmpf:TBITMAPFILEHEADER;
    bmph:TBITMAPINFOHEADER;
    pal:array[0..255] of TRGBQUAD;
    even4_width{width aligned by 4}:longint; pos:longint;
    i:integer; p:pointer;
begin
 loadbmp:=-1;
 seek(f,0);
 fread(f,bmpf,sizeof(bmpf));
 fread(f,bmph,sizeof(bmph));
 if (bmpf.bftype<>'BM') then begin Fileerror('Not a BMP',f); exit; end;
 if (bmph.bibitcount<>8) or (bmph.biplanes<>1) then begin Fileerror('Not a 256 color BMP',f); exit; end;
 if bmph.bicompression<>0 then begin Write('Compressed BMP not supported'); exit; end;
 fread(f,pal,sizeof(pal));
 for i:=0 to 255 do
  with b.pal[i] do
   begin
    red:=pal[i].red;
    green:=pal[i].green;
    blue:=pal[i].blue;
   end;
 b.w:=bmph.biwidth;
 b.h:=bmph.biheight;
 if enoughmem(b.w,b.h,f)=-1 then exit;
 even4_width:=bmph.biwidth div 4*4+ord(bmph.biwidth mod 4<>0)*4;
 b.bits:=heapptr;
 p:=b.bits;
 pos:=filepos(f);
 for i:=1 to b.h do
  begin
   seek(f,pos);
   fread(f,p^,b.w);
   inc(pos,even4_width);
   p:=ip(p,b.w);
  end;
 if errorcode=0 then loadbmp:=0 else Fileerror('Read fault',f);
end;

Function loadPCX(var f:file; var b:tbitmap):integer; {bottom-up bitmap}
type line=array[0..0] of byte;
var
    h:pcxheader;
    pal:tpal;
    pos:longint;
    ptype:byte; {Palette type 10 - 0..63, 12 - 0..255}
    i:integer; p:pointer;
    bp,cw,bw:integer;
    b1,c:byte;

Function getnextbyte:byte;
var c:byte;
begin
if bp<sizeof(buf) then begin getnextbyte:=buf[bp]; inc(bp); end
    else begin fread(f,buf,sizeof(buf)); bp:=1; getnextbyte:=buf[0]; end;
 if inoutres=0 then if errorcode<>0 then;
end;

begin
 loadpcx:=-1;
 fread(f,h,sizeof(h));
 if (h.manuf<>10) and (h.hard<>5) then begin Fileerror('Only PCX version 5 is supported',f); exit; end;
 if (h.bitperpixel<>8) or (h.nplanes<>1) then begin Fileerror('Not a 256 color PCX',f); exit; end;
 seek(f,filesize(f)-sizeof(pal)-sizeof(ptype));
 fread(f,ptype,sizeof(ptype));
 fread(f,pal,sizeof(pal));
 case ptype of
  10: for i:=0 to 255 do
  with b.pal[i] do
   begin
    red:=pal[i].red*4;
    green:=pal[i].green*4;
    blue:=pal[i].blue*4;
   end;
  12: b.pal:=pal;
  else begin Fileerror('Cannot locate palette',f); exit; end;
 end;

 b.w:=h.x2-h.x1+1;
 b.h:=h.y2-h.y1+1;
 if enoughmem(b.w,b.h,f)=-1 then exit;
 b.bits:=heapptr;
 bw:=h.byteperline;
 seek(f,sizeof(h));
 bp:=sizeof(buf);
 for i:=b.h-1 downto 0 do
  begin
   cw:=0;
   while cw<bw do
   begin
    c:=getnextbyte;
    if c<$c0 then begin cbuf[cw]:=c; inc(cw); end
    else
    begin
     b1:=getnextbyte;
     c:=c and 63;
     fillchar(cbuf[cw],c,b1);
     inc(cw,c);
    end;
   end;
   move(cbuf,ip(b.bits,b.w*i)^,b.w);
   if cw<>bw then Writeln('Warning, bad data in file ',getname(f));
   {write(cw-bw,'  '#13); seek(f,filepos(f)+(bw-b.w));}
  end;
 if errorcode=0 then loadpcx:=0 else Fileerror('Read fault',f);
end;



function delt_compress(var ibuf,obuf; size:word; y:word; var maxx:word):word;
var p,sz:word;
var stp{ibuf start position},op{obuf position}:word;
    transparent:byte;
type
	pt=record o,s:word end;
var x,l,ip,obp:word;
    b:^delt_dhead;

function Rle(Var ib,ob;size:word):word;assembler;
var _al:byte;
asm
	push ds
	jmp @start
@store: {ds:bx - from, es:di - to, ax - size}
	push si
	push dx
	push cx
	mov si,bx
	cld
	mov dl,127
	div dl
	mov dh,al
	mov dl,ah
@126:
	or dh,dh
	jz @rest
	mov al,0feh
	stosb
	mov cx,127
	rep movsb
	dec dh
	jmp @126
@rest:
	or dl,dl
	jz @store_out
	mov ch,0
	mov cl,dl
	mov al,dl
	shl al,1
	stosb
	rep movsb
@store_out:
	pop cx
	pop dx
	pop si
	ret
@storerle: {es:di - to, ax:size, _al -byte}
	push dx
	push cx
	mov dl,127
	div dl
	mov dl,ah
	mov ch,0
	mov cl,al
	mov ch,0
	mov al,0ffh
	mov ah,_al
	rep stosw
	or dl,dl
	jz @storerle_out
	mov al,dl
	shl al,1
	or al,1
	mov ah,_al
	stosw
@storerle_out:
	pop cx
	pop dx
	ret
@start:
	lds si,ib
	les di,ob
	mov ax,si
	add ax,size
	mov cx,ax
	mov bx,si
@cyc:
	cmp si,cx
	jae @end
	mov al,[si]
	mov dx,si
	mov _al,al
@scan:
	cmp [si],al
	jnz @endscan
	cmp si,cx
	jae @endscan
	inc si
	jmp @scan
@endscan:
	mov ax,si
	sub ax,dx
	cmp ax,2
	jbe @cyc
	mov ax,dx
	sub ax,bx
	je @nostore
	call @store
@nostore:
	mov ax,si
	sub ax,dx
	call @storerle
	mov bx,si
	jmp @cyc
@end:
	mov ax,si
	sub ax,bx
	jz @1
	call @store
@1:
	mov ax,di
	sub ax,word ptr ob
	pop ds
end;

begin
 ip:=0; obp:=0; b:=@obuf;
 transparent:=lfd_0;
 while ip<size do
 begin


asm
	les di,ibuf
	add di,ip
	mov cx,size
	sub cx,ip
	cld
@find:
	mov al,transparent
	mov dx,di
	repz scasb
	jz @tail
	dec di
	inc cx
@tail:
	mov ax,di
	mov bx,ax
	sub ax,word ptr ibuf
	mov p,ax
	mov al,transparent
	repnz scasb
	jnz @no0
	dec di
	inc cx
@no0:
	mov ax,di
	sub ax,bx
	mov sz,ax
end;

   if sz=0 then break;
   pt(b).o:=ofs(obuf)+obp;
   b^.sizeandtype:=sz shl 1+1;
   b^.startx:=p; b^.starty:=y;
   inc(obp,sizeof(delt_dhead));
   op:=obp;
   l:=rle(tbuf(ibuf)[p],tbuf(obuf)[op],sz);

   if l>=sz {true} then begin move(tbuf(ibuf)[p],tbuf(obuf)[obp],sz); dec(b^.sizeandtype); inc(obp,sz); end
	   else inc(obp,l);
   ip:=p+sz;
 end;
maxx:=pred(ip);
delt_compress:=obp;
end;


function fme_compress(var ibuf,obuf; size:word):word;assembler;
var _di:word;
asm
	push ds
	mov ax,word ptr obuf
	mov _di,ax
	lds si,ibuf
	les di,ibuf
	cld
	mov cx,size
@find0:
	mov al,0
	repnz scasb
	cmp cx,1
	jb @store&exit
	cmp es:[di],al
	jnz @find0
	lea dx,[di-1]
	repz scasb
	jz @zend
	dec di
	inc cx
@zend:
	push di
	push dx
	les di,obuf
	cmp si,dx
	jz @1
	mov ax,dx
	sub ax,si
	call @store
@1:
	pop dx
	pop si
	mov ax,si {si current position}
	sub ax,dx
	call @store0
	mov word ptr obuf,di
	mov di,si
	jmp @find0

@store&exit:
	mov ax,di
	sub ax,si
	les di,obuf
	jz @exit
	call @store
@exit:
	mov ax,di
	sub ax,_di
jmp	@done
@store: {copies AX bytes from DS:SI to ES:DI, breaking them into}
	{<=126 bytes chunks with preceeding length byte. Moves DI}
	push cx
	push si
	cld
	mov dl,127
	div dl
	mov dh,al
	mov dl,ah
@126:
	or dh,dh
	jz @rest
	mov al,127
	stosb
	mov cx,127
	rep movsb
	dec dh
	jmp @126
@rest:
	or dl,dl
	jz @store_out
	mov ch,0
	mov cl,dl
	mov al,dl
	stosb
	rep movsb
@store_out:
	pop si
	pop cx
	ret

@store0:{stores AX zeros to ES:DI, breaking them into}
	{<=127 bytes chunks with preceeding length byte. Moves DI}
	push cx
	mov dl,127
	div dl
	mov ch,0
	mov cl,al
	mov ch,0
	mov al,127+128
	rep stosb
	or ah,ah
	jz @store0_out
	mov al,ah
	add al,128
	stosb
@store0_out:
	pop cx
	ret
@done:
	pop ds
end;

function bm_compress(var ibuf,obuf; size:word):word;assembler;
var _di:word;
asm
	push ds
	mov ax,word ptr obuf
	mov _di,ax
	lds si,ibuf
	les di,ibuf
	cld
	mov cx,size
@find:
	mov al,es:[di]
	mov bl,al
	repnz scasb
	cmp cx,1
	jb @store&exit
	mov ah,al
	cmp es:[di],ax
	jnz @find
	lea dx,[di-1]
	repz scasb
	jz @zend
	dec di
	inc cx
@zend:
	push di
	push dx
	les di,obuf
	cmp si,dx
	jz @1
	mov ax,dx
	sub ax,si
	call @store
@1:
	pop dx
	pop si
	mov ax,si {si current position}
	sub ax,dx
	call @storebl
	mov word ptr obuf,di
	mov di,si
	jmp @find

@store&exit:
	mov ax,di
	sub ax,si
	les di,obuf
	jz @exit
	call @store
@exit:
	mov ax,di
	sub ax,_di
jmp	@done
@store: {copies AX bytes from DS:SI to ES:DI, breaking them into}
	{<=126 bytes chunks with preceeding length byte. Moves DI}
	push cx
	push si
	cld
	mov dl,127
	div dl
	mov dh,al
	mov dl,ah
@126:
	or dh,dh
	jz @rest
	mov al,127
	stosb
	mov cx,127
	rep movsb
	dec dh
	jmp @126
@rest:
	mov ch,0
	mov cl,dl
	mov al,dl
	stosb
	rep movsb
	pop si
	pop cx
	ret
@storebl:{stores AX zeros to ES:DI, breaking them into}
	{<=126 bytes chunks with preceeding length byte. Moves DI}
	push cx
	mov dl,126
	div dl
	mov dl,ah
	mov ch,0
	mov cl,al
	mov ch,0
	mov al,126+128
	mov ah,bl
	rep stosw
	mov al,dl
	add al,128
	mov ah,bl
	stosw
	pop cx
	ret

@done:
	pop ds
end;

(*function bm_compress(var ibuf,obuf; size:word):word;assembler;
var _di:word;
asm
	push ds
	mov ax,word ptr obuf
	mov _di,ax
	lds si,ibuf
	les di,ibuf
	cld
	mov cx,size
@find:
	cmp cx,3
	jbe @store&exit
	mov al,es:[di]
	inc di
	dec cx
	mov ah,al
	cmp ax,es:[di]
	jnz @find
	lea dx,[di-1]
	repz scasb
	jz @compr
	dec di
	inc cx
@compr:
	mov bl,al
	push di
	push dx
	les di,obuf
	cmp si,dx
	jz @1
	mov ax,dx
	sub ax,si
	call @store
@1:
	pop dx
	pop si
	mov ax,si {si current position}
	sub ax,dx
	call @storebl
	mov word ptr obuf,di
	mov di,si
	jmp @find

@store&exit:
	mov ax,di
	sub ax,si
	les di,obuf
	jz @exit
	call @store
@exit:
	mov ax,di
	sub ax,_di
jmp	@done
@store: {copies AX bytes from DS:SI to ES:DI, breaking them into}
	{<=126 bytes chunks with preceeding length byte. Moves DI}
	push cx
	push si
	cld
	mov dl,126
	div dl
	mov dh,al
	mov dl,ah
@126:
	or dh,dh
	jz @rest
	mov al,126
	stosb
	mov cx,126
	rep movsb
	dec dh
	jmp @126
@rest:
	mov ch,0
	mov cl,dl
	mov al,dl
	stosb
	rep movsb
	pop si
	pop cx
	ret

@storebl:{stores AX BLs to ES:DI, breaking them into}
	{<=126 bytes chunks with preceeding length byte. Moves DI}
	mov dl,126
	div dl
	mov dl,al
@2:
	or dl,dl
	jz @3
	mov al,126+128
	stosb
	mov al,bl
	stosb
	dec dl
	jmp @2
@3:
	mov al,ah
	add al,128
	stosb
	mov al,bl
	stosb
	ret
@done:
	pop ds
end; *)

procedure convertpalette(var bm:tbitmap; var pal:tpal);
var
	ctable:array[0..255] of byte;
	i,j,d,d1:integer;p:pointer;
	cts,w:word; r,g,b:byte;
	firstcolor,lastcolor:byte;
	skip32:boolean;
	offs32:word;
	firstoffs:word;
	do_skip:boolean;
	skipcolor:byte;


function m(i:integer):integer;
begin
 if i<0 then m:=-i else m:=i;
end;

begin
 write('Converting palette...');
{match colors}
do_skip:=not lfd_use_0; skipcolor:=lfd_0;
if use0 then firstcolor:=0
else if use32 then firstcolor:=1 else firstcolor:=32;
firstoffs:=ofs(pal)+firstcolor*sizeof(pal[1]);
offs32:=ofs(pal)+32*sizeof(pal[1]);
skip32:=not use32;
 for i:=0 to 255 do
 begin
   r:=bm.pal[i].red;g:=bm.pal[i].green;b:=bm.pal[i].blue;
 asm
	push ds
	lds si,pal
	lea bx,ctable
	add bx,i
	mov d,0ffffh
	mov ch,0
	mov cl,firstcolor
	mov si,firstoffs
	cld
 @cyc:
	cmp do_skip,0
	jz @no_skip
	cmp cl,skipcolor
	jnz @no_skip
	inc cx
	add si,3
	jmp @cyc
@no_skip:
	cmp skip32,0
	jz @9       {skip colors 1-31}
	cmp cl,32
	ja @9
	cmp cl,0
	jz @match
	jmp @to32
@9:
	cmp cl,31
	ja @match  {skip colors 24-31}
	cmp cl,24
	jb @match
 @to32:
	mov cl,32
	mov si,offs32
 @match:
	xor dx,dx
	xor ax,ax
	lodsb
	sub al,r
	jnc @1
	neg al
 @1:
	add dx,ax
	lodsb
	sub al,g
	jnc @2
	neg al
 @2:
	add dx,ax
	lodsb
	sub al,b
	jnc @3
	neg al
 @3:
	add dx,ax
	cmp dx,d
	jae @4
	mov d,dx
	mov ss:[bx],cl
 @4:
	cmp cl,255
	jae @exit
	inc cx
	jmp @cyc
 @exit:
	pop ds
 end;
   { ctable[i]:=0;
    r:=bm.pal[i].red;g:=bm.pal[i].green;b:=bm.pal[i].blue;
    d:=m(r-pal[0].red)+m(g-pal[0].green)+m(b-pal[0].blue);
  for j:=1 to 255 do
   begin
    d1:=m(r-pal[j].red)+m(g-pal[j].green)+m(b-pal[j].blue);
    if d1<d then begin ctable[i]:=j; d:=d1; end;
   end;}
 end;
{convert bitmap}
bm.pal:=pal;
cts:=seg(ctable);
w:=bm.w;
for i:=0 to bm.h-1 do
 begin
   p:=ap(pa(bm.bits)+i*bm.w);
  asm
	push ds
	les di,p
	mov ds,cts
	lea bx,ctable
	mov cx,w
	cld
  @1:
	mov al,es:[di]
	xlat
	stosb
	loop @1
	pop ds
  end;
 end;
writeln;
end;

function DELT_write(var f:file;var b:tbitmap):integer;
var h:delt_head;a:word;i:integer;mx:word;
    cpos,hpos:longint;
    ph:delt_dhead absolute cbuf;
begin
hpos:=filepos(f);
delt_write:=-1;
h.offsx:=0;
h.offsy:=0;
h.maxx:=b.w-1;
h.maxy:=b.h-1;
fwrite(f,h,sizeof(h));
fillchar(h,sizeof(h),255);
for i:=0 to b.h-1 do
begin
 a:=delt_compress(ip(b.bits,(b.h-i-1)*b.w)^,cbuf,b.w,i,mx);
 if a<>0 then
 begin
  if h.offsy=65535 then h.offsy:=i;
  h.maxy:=i;
  if h.offsx>ph.startx then h.offsx:=ph.startx;
  if h.maxx=65535 then h.maxx:=mx;
  if mx>h.maxx then h.maxx:=mx;
 end;
 fwrite(f,cbuf,a);
end;
i:=0;
fwrite(f,i,2);
if h.offsx=65535 then fillchar(h,sizeof(h),0);
cpos:=filepos(f); seek(f,hpos); fwrite(f,h,sizeof(h)); seek(f,cpos);
if errorcode=0 then delt_write:=0 else FileError('Write fault',f)
end;


Function FMEwrite(var f:file; var b:tbitmap; d:fme_details):integer;
var
	h1:FME_header1;
	h2:FME_header2;

procedure FMEwriteuncompressed;
begin
 flipwrite(f,b);
end;

Procedure FMEwritecompressed;
var i:integer;
    ofs_pos{position of offset table},cpos{current position},
    offs{value of current offset}:longint;
    len{length of current compressed column}:word;
begin
 ofs_pos:=sizeof(h1)+sizeof(h2);
 cpos:=ofs_pos+b.w*4;
 seek(f,cpos);
 for i:=0 to b.w-1 do
  begin
   offs:=cpos-sizeof(FME_header1);
   seek(f,ofs_pos+i*4); fwrite(f,offs,sizeof(offs));
   seek(f,cpos);
   getcolumn(b,i,buf);
   len:=fme_compress(buf,cbuf,b.h);
   fwrite(f,cbuf,len);
   inc(cpos,len);
  end;
  h2.datasize:=cpos-h1.header2;
  seek(f,sizeof(h1)); fwrite(f,h2,sizeof(h2));
end;

begin
 fmewrite:=-1;
 Write('Writing ',getname(f),':');
 if d.compressed then writeln(' Compressed') else writeln(' Uncompressed');
h1.xshift:=d.xshift;
h1.yshift:=d.yshift;
h1.flip:=d.flip;
h1.header2:=sizeof(FME_header1);
fillchar(h1.fill,sizeof(h1.fill),0);
h2.sizex:=b.w;
h2.sizey:=b.h;
h2.compressed:=ord(d.compressed);
h2.datasize:=b.w*b.h;
fillchar(h2.fill,sizeof(h2.fill),0);
fwrite(f,h1,sizeof(h1));
fwrite(f,h2,sizeof(h2));
if d.compressed then FMEwritecompressed else FMEwriteuncompressed;
if errorcode=0 then fmewrite:=0 else Fileerror('Write fault',f);
end;

procedure BMwritemagic(var f:file);
var bm:bm_magic;
begin
bm:='BM'#32#30;
fwrite(f,bm,sizeof(bm));
end;

procedure BMfillmheader(var h:bm_mheader;var b:tbitmap; var md:bm_details);
begin
 fillchar(h,sizeof(h),0);
 h.sizex:=b.w;
 h.sizey:=b.h;
 h.idemx:=h.sizex;
 h.idemy:=h.sizey;
 h.datasize:=longint(h.sizex)*longint(h.sizey);
 h.logsizey:=lg2(h.sizey);
 h.ib:=$74;
 h.iw:=$4431;
 if md.transparent then h.transparent:=$3e else h.transparent:=$36;
end;

procedure BMfillheader(var bmh:bm_header;var b:tbitmap;var d:bm_details);
begin
bmh.sizex:=b.w;
bmh.sizey:=b.h;
bmh.idemx:=bmh.sizex;
bmh.idemy:=bmh.sizey;
if d.transparent then bmh.transparent:=$3e else bmh.transparent:=$36;
bmh.logsizey:=lg2(bmh.sizey);
bmh.compressed:=d.compressed;
bmh.datasize:=longint(bmh.sizex)*longint(bmh.sizey);
fillchar(bmh.filler,12,0);
end;

function BMwritedata(var f:file;var b:tbitmap; d:bm_details; multi:boolean):integer;
var
    bpos:longint;

Procedure bm_writecompressed;
var i:integer;
    dsize_pos,ipos,cpos{current position}:longint;
    len{length of current compressed column}:word;
begin
 if b.w>maxwidth then begin Fileerror('width>6000 - can''t write compressed BM',f); exit; end;
 ipos:=filepos(f); cpos:=ipos;
 for i:=0 to b.w-1 do
  begin
   cols[i]:=cpos-ipos;
   getcolumn(b,i,buf);
   case d.compressed of
    1:len:=bm_compress(buf,cbuf,b.h);
    2:len:=fme_compress(buf,cbuf,b.h);
    else exit;
   end;
   fwrite(f,cbuf,len);
   inc(cpos,longint(len));
  end;
  dsize_pos:=cpos-ipos;
  if multi then dec(ipos,20) else dec(ipos,16);
  seek(f,ipos); fwrite(f,dsize_pos,sizeof(dsize_pos));
  seek(f,cpos);
  fwrite(f,cols,b.w*4);
end;

begin
bmwritedata:=-1;
bpos:=filepos(f);
if d.compressed<>0 then bm_writecompressed else flipwrite(f,b);
if errorcode=0 then bmwritedata:=0 else Fileerror('Write fault',f);
end;

Function bmwrite(var f:file;var b:tbitmap; d:bm_details):integer;
var bmh:bm_header;
begin
Write('Writing ',getname(f),':');
if d.transparent then write(' Transparent,');
if d.compressed<>0 then writeln(' Coding ',d.compressed) else writeln(' Uncompressed');
BMwritemagic(f);
BMfillheader(bmh,b,d);
fwrite(f,bmh,sizeof(bmh));
bmwrite:=bmwritedata(f,b,d,false);
end;

procedure doshow(var b:tbitmap);
var x,y:integer; c,c1:byte;
label noredraw;

procedure blt(var b:tbitmap;x,y:word);
var w,h:word;
i:integer;
begin
 w:=b.w-x;
 if w>320 then w:=320;
 h:=b.h-y;
 if h>200 then h:=200;
 for i:=0 to h-1 do move(ip(b.bits,(b.h-i-y-1)*b.w+x)^,mem[$a000:i*320],w);
end;

procedure setpal;
var i:integer; s:word;
var pal:tpal;
begin
for i:=0 to 255 do
with pal[i] do
begin
red:=b.pal[i].red div 4;
green:=b.pal[i].green div 4;
blue:=b.pal[i].blue div 4;
end;
s:=seg(pal);
asm
	mov ax,1012h
	mov es,s
	lea dx,pal
	mov bx,0
	mov cx,256
	int 10h
end;
end;


var
 xscr,yscr:boolean;

begin
 x:=0; y:=0;
 xscr:=b.w>320;
 yscr:=b.h>200;
 asm
	mov ax,13h
	int 10h
 end;
 setpal;
 blt(b,x,y);
repeat
asm
	mov ah,0
	int 16h
	mov c,al
	mov c1,ah
end;
case c1 of
	71:{home} begin x:=0; y:=0; end;
	72:{up} if y>0 then dec(y);
	73:{pgup} if y>200 then dec(y,200) else y:=0;
	75:{<-} if x>0 then dec(x);
	77:{->} if xscr then if x>b.w-320 then inc(x);
	79:{end} begin if yscr then y:=b.h-200; if xscr then x:=b.w-320; end;
	80:{down} if yscr then if y<b.h-200 then inc(y);
	81:{pgdn} if y<b.h-400 then inc(y,200) else y:=b.h-200;
    else goto noredraw;
end;
blt(b,x,y);
noredraw:
until (c=27) or (c=32) or (c=13);
 asm
	mov ax,3
	int 10h
end;
end;

