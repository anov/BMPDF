const err:byte=0;
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

procedure fdelete(var f:file);
begin
 close(f); if ioresult=0 then;
 Writeln('Deleting ',getname(f));
 erase(f);
end;

procedure Fileerror(s:string;var f:file);
begin
 Writeln('Error: ',s,' file: ',getname(f));
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


procedure convertpalette(var bm:tbitmap; var pal:tpal);
var
	ctable:array[0..255] of byte;
	i,j,d,d1:integer;p:pointer;
	cts,w:word; r,g,b:byte;
	firstcolor:byte;
	skip32:boolean;
	offs32:word;
	firstoffs:word;

function m(i:integer):integer;
begin
 if i<0 then m:=-i else m:=i;
end;

begin
 write('Converting palette...');
{match colors}
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
	{<=126 bytes chunks with preceeding length byte. Moves DI}
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


Function cell_write(var f:file; var b:tbitmap; var cell:cell_data):integer;
var
	h1:FME_header1;
	h2:FME_header2;
    ipos:longint;
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
 ofs_pos:=filepos(f); {sizeof(h1)+sizeof(h2);}
 cpos:=ofs_pos+b.w*4;
 seek(f,cpos);
 for i:=0 to b.w-1 do
  begin
   offs:=cpos-ipos;
   seek(f,ofs_pos+i*4); fwrite(f,offs,sizeof(offs));
   seek(f,cpos);
   getcolumn(b,i,buf);
   len:=fme_compress(buf,cbuf,b.h);
   fwrite(f,cbuf,len);
   inc(cpos,len);
  end;
  h2.datasize:=cpos-ipos;
  seek(f,ipos); fwrite(f,h2,sizeof(h2));
  seek(f,cpos);
end;

begin
 cell_write:=-1;
ipos:=filepos(f);
h2.sizex:=b.w;
h2.sizey:=b.h;
h2.compressed:=cell.compressed;
h2.datasize:=b.w*b.h;
fillchar(h2.fill,sizeof(h2.fill),0);
fwrite(f,h2,sizeof(h2));
if h2.compressed<>0 then FMEwritecompressed else FMEwriteuncompressed;
if errorcode=0 then cell_write:=0 else Fileerror('Write fault',f);
end;


Procedure setgraphics;
begin
if not graphics then
begin
 graphics:=true;
 asm
	mov ax,13h
	int 10h
 end;
end;
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

procedure doshow(var b:tbitmap);
const
	lastw:integer=320;
	lasth:integer=200;

var x,y:integer; c,c1:byte;
label noredraw;

Procedure rect(x,y,w,h,c:integer);
var i:integer;
begin
 for i:=0 to h-1 do Fillchar(mem[$a000:(y+i)*320+x],w,c);
end;

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
 if lastw>b.w then rect(b.w,0,lastw-b.w,320-b.h,0);
 if lasth>b.h then rect(0,b.h,320-b.w,lasth-b.h,0);
 lastw:=b.w;
 lasth:=b.h;

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
if c=3 then begin settext; Writeln('Terminated'); halt; end;
until (c=27) or (c=32) or (c=13);
end;
