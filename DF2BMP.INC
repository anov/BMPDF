const	err:byte=0;
	graphics:boolean=false;


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

Procedure settext;
begin
if graphics then
begin
 graphics:=false;
 asm
	mov ax,3h
	int 10h
 end;
end;
end;

procedure Fileerror(s:string;var f:file);
begin
 settext;
 Writeln('Error: ',s,' file: ',getname(f));
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

procedure setcolumn(b:tbitmap; n:word; var buf);
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
	les di,p
	lds si,buf
	mov bx,n
	mov dx,h16
	cld
	or dx,dx
	jz @rest
  @cyc:
	mov cx,16
  @16:
	lodsb
	mov es:[bx+di],al
	add di,w
	loop @16
	mov ax,es
	add ax,w
	mov es,ax
	mov di,offs
	dec dx
	jnz @cyc
  @rest:
	mov cx,r16
	or cx,cx
	jz @done
  @cyc1:
	lodsb
	mov es:[bx+di],al
	add di,w
	loop @cyc1
  @done:
	pop ds
  end;
{  for j:=0 to w-1 do plane[j]:=byte(ip(b,j*h+i)^);}
end;

function flipread(var f:file; var b:tbitmap):integer;
var i:integer;
begin
 for i:=0 to b.w-1 do
  begin
   fread(f,buf,b.h);
   setcolumn(b,i,buf);
  end;
end;


Function SaveBMP(var f:file; var b:tbitmap):integer; {bottom-up bitmap}
var
    bmpf:TBITMAPFILEHEADER;
    bmph:TBITMAPINFOHEADER;
    pal:array[0..255] of TRGBQUAD;
    even4_width{width aligned by 4}:longint; pos:longint;
    i:integer; p:pointer;

function even4(value:word):word;
begin
 even4:=(value div 4)*4+ord(value mod 4<>0)*4;
end;

begin
 savebmp:=-1;
 even4_width:=even4(b.w);
 with bmpf do
 begin
  bftype:='BM';
  bfreserved1:=0;
  bfreserved2:=0;
  bfsize:=sizeof(bmph)+sizeof(pal)+sizeof(bmpf)+even4_width*b.h;
  bfoffbits:=sizeof(bmpf)+sizeof(bmph)+sizeof(pal);
 end;
 with bmph do
 begin
  bisize:=sizeof(bmph);
  biwidth:=b.w;
  biheight:=b.h;
  biplanes:=1;
  bibitcount:=8;
  bicompression:=0;
  bisizeimage:=0;
  bixpelpermeter:=100;
  biypelpermeter:=100;
  biclrused:=256;
  biclrimport:=256;
 end;
 for i:=0 to 255 do
		  begin
		   pal[i].red:=b.pal[i].red;
		   pal[i].green:=b.pal[i].green;
		   pal[i].blue:=b.pal[i].blue;
		   pal[i].reservd:=0;
		  end;
fwrite(f,bmpf,sizeof(bmpf));
fwrite(f,bmph,sizeof(bmph));
fwrite(f,pal,sizeof(pal));
for i:=0 to b.h-1 do
begin
 p:=ap(pa(b.bits)+i*b.w);
 fwrite(f,p^,even4_width);
end;
if errorcode=0 then savebmp:=0 else Fileerror('Write fault',f);
end;

function pcx_compress(Var ib,ob;size:word):word;assembler;
var _al:byte;
asm
	push ds
	jmp @start
@store: {ds:bx - from, es:di - to, ax - size}
	push si
	push dx
	push cx
	mov si,bx
	mov cx,ax
	cld
@loop1:
	lodsb
	stosb
	cmp al,0C0h
	jb @nextbyte
	mov byte ptr es:[di-1],0C1h
	stosb
@nextbyte:
	loop @loop1
	pop cx
	pop dx
	pop si
	ret
@storerle: {es:di - to, ax:size, _al -byte}
	push dx
	push cx
	mov dl,63
	div dl
	mov dl,ah
	mov ch,0
	mov cl,al
	mov ch,0
	mov al,0ffh
	mov ah,_al
	rep stosw
	or dl,dl
	jz @9
	mov al,dl
	or al,0C0h
	mov ah,_al
	stosw
@9:
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
	cmp ax,1
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



Function SavePCX(var f:file; var b:tbitmap):integer; {bottom-up bitmap}
type line=array[0..0] of byte;
var
    h:pcxheader;
    pos:longint;
    ptype:byte; {Palette type 10 - 0..63, 12 - 0..255}
    i:integer; p:pointer;
    bw:integer;

begin
 savepcx:=-1;
 h.manuf:=10;
 h.hard:=5;
 h.encod:=1;
 h.bitperpixel:=8;
 h.x1:=0; h.y1:=0;
 h.x2:=b.w-1; h.y2:=b.h-1;
 h.hres:=320;
 h.vres:=200;
 fillchar(h.palette,sizeof(h.palette),0);
 h.vmode:=$13;
 h.nplanes:=1;
 h.byteperline:=b.w;
 h.palinfo:=1;
 h.shres:=300; h.svres:=300;
 fillchar(h.extra,sizeof(h.extra),0);
 fwrite(f,h,sizeof(h));
 for i:=b.h-1 downto 0 do
 begin
  bw:=pcx_compress(ip(b.bits,b.w*i)^,cbuf,b.w);
  fwrite(f,cbuf,bw);
 end;
 ptype:=12;
 fwrite(f,ptype,1);
 fwrite(f,b.pal,sizeof(b.pal));
 if errorcode=0 then savepcx:=0 else Fileerror('Write fault',f);
end;


function bm_uncompress (var ibuf,obuf; size:word):word;assembler;
var _di:word;
asm
	push ds
	lds si,ibuf
	les di,obuf
	mov _di,di
	lea bx,[di+maxheight-256]
	cld
	mov dx,si
	add dx,size
	xor cx,cx
@cyc:
	cmp si,dx
	jae @okexit
	cmp di,bx
	jae @badexit
	lodsb
	cmp al,0ffh
	jz @okexit
	test al,80h
	jnz @fill
	mov cl,al
	rep movsb
	jmp @cyc
@fill:
	and al,7Fh
	mov cl,al
	lodsb
	rep stosb
	jmp @cyc
@badexit:
	mov ax,-1
	jmp @exit
@okexit:
	mov ax,di
	sub ax,_di
@exit:
	pop ds
end;

function fme_uncompress(var ibuf,obuf; size:word):word;assembler;
var _di:word;
asm
	push ds
	lds si,ibuf
	les di,obuf
	mov _di,di
	lea bx,[di+maxheight-256]
	cld
	mov dx,si
	add dx,size
	xor cx,cx
@cyc:
	cmp si,dx
	jae @okexit
	cmp di,bx
	jae @badexit
	lodsb
	test al,80h
	jnz @zeros
	mov cl,al
	rep movsb
	jmp @cyc
@zeros:
	and al,7Fh
	mov cl,al
	mov al,0
	rep stosb
	jmp @cyc
@badexit:
	mov ax,-1
	jmp @exit
@okexit:
	mov ax,di
	sub ax,_di
@exit:
	pop ds
end;

Function FMEload(var f:file; var b:tbitmap;var fd:fme_details):integer;
var
	h1:FME_header1;
	h2:FME_header2;

procedure FMEloaduncompressed;
begin
 flipread(f,b);
end;

Procedure FMEloadcompressed;
var i:integer;
    ofs_pos{position of offset table},
    offs{value of current offset},next:longint;
    len{length of current compressed column}:word;
begin
 ofs_pos:=h1.header2+sizeof(h2);
 for i:=0 to b.w-1 do
  begin
   seek(f,ofs_pos+i*4);
   fread(f,offs,sizeof(offs));
   inc(offs,h1.header2);
   if i=b.w-1 then next:=h2.datasize
	      else fread(f,next,sizeof(next));
   inc(next,h1.header2);
   len:=next-offs;
   seek(f,offs);
   fread(f,cbuf,len);
   fillchar(buf,b.h,0);
   if fme_uncompress(cbuf,buf,len)<>b.h then writeln('Warning: collumn length mismatch ',getname(f));
   setcolumn(b,i,buf);
  end;
end;

begin
fmeload:=-1;
fread(f,h1,sizeof(h1));
seek(f,h1.header2);
fread(f,h2,sizeof(h2));
fd.compressed:=h2.compressed<>0;
fd.xshift:=h1.xshift;
fd.yshift:=h1.yshift;
b.w:=h2.sizex;
b.h:=h2.sizey;
if enoughmem(b.w,b.h,f)=-1 then exit;
b.bits:=heapptr;
if h2.compressed=1 then FMEloadcompressed else FMEloaduncompressed;
 if errorcode=0 then fmeload:=0 else Fileerror('Read fault',f);
end;

function BMreadmagic(var f:file):integer;
var    m:BM_magic;
begin
bmreadmagic:=-1;
fread(f,m,sizeof(m));
if m='BM'#32#30 then bmreadmagic:=0;
end;

procedure BMreadheader(var f:file;var h);
begin
fread(f,h,sizeof(bm_header));
end;

type bmtype=(simple,multi);
function BMreaddata(var f:file;var h;var b:tbitmap;btype:bmtype):integer;
var ct:byte;
procedure bm_readcompressed(var f:file;var b:tbitmap;var h:bm_header);
var i:integer;
    ipos,ofs_pos{position of offset table},
    offs{value of current offset},next:longint;
    len{length of current compressed column}:word;
    dsize:longint;
begin
 if btype=multi then dsize:=bm_mheader(h).datasize else dsize:=h.datasize;
 ipos:=filepos(f);
 ofs_pos:=ipos+dsize;
 for i:=0 to b.w-1 do
  begin
   seek(f,ofs_pos+i*4);
   fread(f,offs,sizeof(offs));
   inc(offs,ipos);
   if i=b.w-1 then next:=dsize
	      else fread(f,next,sizeof(next));
   inc(next,ipos);
   len:=next-offs;
   seek(f,offs);
   fread(f,cbuf,len);
   fillchar(buf,b.h,0);
   case ct of
    1: if bm_uncompress(cbuf,buf,len)<>b.h then writeln('Warning: collumn length mismatch ',getname(f));
    2: if fme_uncompress(cbuf,buf,len)<>b.h then writeln('Warning: collumn length mismatch ',getname(f));
   else begin Fileerror('Unknown compression method',f); exit; end;
   end;
   setcolumn(b,i,buf);
  end;
end;


begin
bmreaddata:=-1;
ct:=0;
if btype=multi then
with bm_mheader(h) do
begin
 ct:=0;
 b.w:=sizex;
 b.h:=sizey;
end
else
with bm_header(h) do
begin
 ct:=compressed;
{ if compressed<>0 then begin Fileerror('Compressed BM not supported',f);exit; end;}
 b.w:=sizex;
 b.h:=sizey;
end;
b.bits:=heapptr;
if enoughmem(b.w,b.h,f)=-1 then exit;
if ct=0 then flipread(f,b) else bm_readcompressed(f,b,bm_header(h));
if errorcode=0 then bmreaddata:=0;
end;

Function delt_load(var f:file;var b:tbitmap;delt_size:longint):integer;
var
    h:delt_head;
    d:delt_dhead;
    n:integer;
    c:byte;
    o:integer;
    bpos,bsize:word;
    fsize,fbytes,fpos:longint;
    i:integer;

Function XYLok(x,y,l:integer):boolean;
begin
 xylok:=false;
 if (x<h.offsx) or (x>h.maxx) or (y<h.offsy) or (y>h.maxy) then exit;
 if (x+l-1>h.maxx) then exit;
 xylok:=true;
end;

procedure fbufread(var f:file;var b;size:word);
var p:pointer; a:word;
begin
if bpos+size>bsize then
begin
 p:=@b;
 a:=bsize-bpos;
 move(cbuf[bpos],p^,a);
 inc(longint(p),a);
 if fbytes>sizeof(cbuf) then bsize:=sizeof(cbuf) else bsize:=fbytes;
 fread(f,cbuf,bsize);
 dec(fbytes,bsize);
 bpos:=0;
 move(cbuf[0],p^,size-a);
 inc(bpos,size-a);
end
else
begin
 move(cbuf[bpos],b,size);
 inc(bpos,size);
end;
inc(fpos,size);
end;

procedure firstread(var f:file;var b;size:word);
begin
fbytes:=delt_size;
bsize:=fbytes; bpos:=fbytes;
fpos:=0;
fbufread(f,b,size);
end;

procedure draw(var b:tbitmap;x,y:word;var bf;size:word);
var p:pointer;
begin
 p:=ip(b.bits,longint(b.h-y-1)*b.w+x);
 asm
	push ds
	les di,p
	lds si,bf
	mov cx,size
	cld
	rep movsb
	pop ds
 end;
end;

function drawcolor(var b:tbitmap;x,y:word;c:byte;size:word):word;
var p:pointer;
begin
 p:=ip(b.bits,longint(b.h-y-1)*b.w+x);
 asm
	les di,p
	mov cx,size
	cld
	mov al,c
	rep stosb
 end;
end;


begin
 delt_load:=-1;
 firstread(f,h,sizeof(h));
 b.w:=h.maxx+1;
 b.h:=h.maxy+1;
 b.bits:=heapptr;
{asm
 mov ax,13h
 int 10h
end;
 b.w:=320;
 b.h:=200;
 b.bits:=ptr($a000,0);}

 if enoughmem(b.w,b.h,f)=-1 then exit;
 for i:=0 to b.h-1 do drawcolor(b,0,i,lfd_0,b.w);
 fsize:=filesize(f);
 while fpos<fsize do
 begin
  fbufread(f,d.sizeandtype,2);
  if d.sizeandtype=0 then break else fbufread(f,d.startx,sizeof(d)-2);
  if not xylok(d.startx,d.starty,d.sizeandtype shr 1) then begin Fileerror('Invalid picture - out of bounds',f); exit;end;
  if d.sizeandtype and 1=0 then
  begin
   n:=d.sizeandtype shr 1;
   if n>b.w then begin
			Fileerror('Bad data',f); exit;
		     end;
   fbufread(f,buf[0],n);
   draw(b,d.startx,d.starty,buf,n);
  end
  else
  begin
   n:=d.sizeandtype shr 1;
   o:=0;
   while o<n do
   begin
    fbufread(f,c,1);
    if c and 1=0 then
    begin
     fbufread(f,buf,c shr 1);
     draw(b,d.startx+o,d.starty,buf,c shr 1);
     inc(o,c shr 1);
    end
    else
    begin
      fbufread(f,buf,1);
      drawcolor(b,d.startx+o,d.starty,buf[0],c shr 1);
      inc(o,c shr 1);
    end;
   end;
  if o<>n then Fileerror('Suspicious data, may lock up',f);
  end;
 end;
if errorcode=0 then delt_load:=0 else Fileerror('Read fault',f);
end;

Procedure setgraphics;
begin
if graphics then fillchar(mem[$a000:0],64000,0) else
begin
 graphics:=true;
 asm
	mov ax,13h
	int 10h
 end;
end;
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
 setgraphics;
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
	77:{->} if xscr then if x+320<b.w then inc(x);
	79:{end} begin if yscr then y:=b.h-200; if xscr then x:=b.w-320; end;
	80:{down} if yscr then if y<b.h-200 then inc(y);
	81:{pgdn} if yscr then if y+400<b.h then inc(y,200) else y:=b.h-200;
    else goto noredraw;
end;
blt(b,x,y);
noredraw:
until (c=27) or (c=32) or (c=13);
if c=27 then begin settext; halt; end;
end;
