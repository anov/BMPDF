{$A+,B-,D+,E-,F-,G-,I-,L+,N-,O-,P-,Q-,R-,S-,T-,V-,X+}
{$M 65000,0,655360}

const
	nline:word=0;
	maxheight=6000;
	maxwidth=6000;
	idir:string[68]='';
	odir:string[68]='';
	pdir:string[68]='';
	pic_type:byte=0;
	lfd_0:byte=0;

{$i types.inc}
var
    error:boolean;
    pm:pmrecord;
    w,s:string;
    file1,file2,file3:string[80];
    pal:tpal;
    f,fout:file;
    b:tbitmap;
    buf:array[0..maxheight-1] of byte;
    cbuf:array[0..maxheight-1] of byte;
    cols:array[0..maxwidth-1] of longint;

{$i bmp2df.inc}
{$i parse.inc}

procedure makeerror(s:string);
begin
 if s<>'' then write('Syntax: ',s);
 reportline;
end;

procedure smartguess(var p:pmrecord);
var e:ext;firstpal:boolean;
pfile,ofile:fname;
begin
 ofile:=''; pfile:='';
 firstpal:=false;
 if p.files[1]='' then begin get8name(p.files[0],p.files[1]); exit; end;
 e:=getext(p.files[1]);slocase(e);
 if (e='.pal') or (e='.plt') then begin pfile:=p.files[1]; firstpal:=true; end;
 if p.files[2]='' then if firstpal then get8name(p.files[0],ofile)
				   else ofile:=p.files[1]
		  else begin ofile:=p.files[1]; pfile:=p.files[2]; end;
 p.files[1]:=ofile;
 p.files[2]:=pfile;
end;


function loadlfdpal(var s:string):integer;
var p:file; i:integer;
   pl:lfd_pal;
begin
loadlfdpal:=-1;
if popenread(p,s,'.plt')=-1 then exit;
fread(p,pl,2);
if (pl.first<>0) or (pl.last<>255) then begin Fileerror('Not full pallette',p); close(p); exit; end;
fread(p,pal,sizeof(pal));
close(p);
if errorcode=0 then loadlfdpal:=0 else Fileerror('Read fault',p);
end;

function loadpal(var s:string;de:ext):integer;
var p:file; i:integer;e:ext;
begin
e:=getext(s); slocase(e);
if de='.plt' then if e<>'.pal' then begin loadpal:=loadlfdpal(s); exit; end;
if e='.plt' then begin loadpal:=loadlfdpal(s); exit; end;
loadpal:=-1;
if popenread(p,s,de)=-1 then exit;
fread(p,pal,sizeof(pal));
close(p);
if errorcode<>0 then exit;
for i:=0 to 255 do
 begin
  pal[i].red:=pal[i].red*4;
  pal[i].green:=pal[i].green*4;
  pal[i].blue:=pal[i].blue*4;
 end;
loadpal:=0;
end;

function loadbitmap(var f:file;var b:tbitmap):integer;
var e:ext;tp:byte;
begin
e:=getext(getname(f));slocase(e);
if pic_type<>0 then tp:=pic_type else if e='.pcx' then tp:=2 else tp:=1;
if tp=2 then loadbitmap:=loadPCX(f,b) else loadbitmap:=loadBMP(f,b);
end;

procedure getbd(p:pmrecord;var bd:bm_details);
begin
 if pos('2',p.sw)<>0 then bd.compressed:=2 else
  if pos('1',p.sw)<>0 then bd.compressed:=1 else bd.compressed:=0;
 bd.transparent:=pos('T',p.sw)<>0;
end;

procedure message(var t:text;var p:pmrecord);
begin
 repeat
  getline(t,s,false);
  writeln(s);
 until s='';
end;

procedure setvars(var t:text;var p:pmrecord);
begin
 if p.files[1]='INDIR' then
 begin
  idir:=p.files[2];
  addslash(idir);
 end else
 if p.files[1]='OUTDIR' then
 begin
   odir:=p.files[2];
   addslash(odir);
 end else
 if p.files[1]='PALDIR' then
 begin
   pdir:=p.files[2];
   addslash(pdir);
 end
 else makeerror('Invalid SET statement ');
end;


procedure simple(var t:text;var p:pmrecord);
var f:file;
    bd:BM_details;
procedure a;
begin
if openwrite(fout,p.files[1],'.bm')=-1 then begin reportline; exit; end;
getline(t,s,true); parse(s,pm);
use32:=pos('L',pm.sw)<>0;
use0:=pos('S',pm.sw)=0;
getbd(pm,bd);
if openread(f,p.files[0],'.bmp')=-1 then begin reportline; exit; end;
if loadbitmap(f,b)=-1 then begin reportline; exit; end;
if p.files[1]='' then else if loadpal(p.files[1],'.pal')=-1 then
 begin
  reportline; exit;
 end else convertpalette(b,pal);
if BMWrite(fout,b,bd)=-1 then begin reportline; exit; end;
error:=false;
end;

begin
 error:=true;
 a;
 close(f); close(fout);
 if errorcode<>0 then;
 if error then fdelete(fout);
 skip2next(t);
end;

Procedure getfd(var p:pmrecord; var fd:fme_details);
begin
 fd.flip:=0;
 fd.compressed:=pos('U',p.sw)=0;
end;

procedure Frame(var t:text;var p:pmrecord);
var fd:FME_details;l:longint;i:integer;
    xchange,ychange:boolean;
procedure a;
begin
 xchange:=true; ychange:=true;
 if p.files[2]<>'' then
 begin
  val(p.files[2],l,i);
  if i<>0 then begin makeerror('Bad xshift value: '+p.files[2]);end
  else begin fd.xshift:=l; xchange:=false; end;
 end;
 if p.files[3]<>'' then
 begin
  val(p.files[3],l,i);
  if i<>0 then begin makeerror('Bad yshift value: '+p.files[3]);end
  else begin fd.yshift:=l; ychange:=false; end;
 end;

if openwrite(fout,p.files[1],'.fme')=-1 then begin reportline; exit; end;
 getline(t,s,true); parse(s,p); getfd(p,fd);
 use32:=pos('L',p.sw)<>0;
 use0:=pos('S',p.sw)=0;

if openread(f,p.files[0],'.bmp')=-1 then begin reportline; exit; end;
if loadbitmap(f,b)=-1 then begin reportline; exit; end;
if xchange then fd.xshift:=-(b.w shr 1);
if ychange then fd.yshift:=-b.h;
if p.files[1]='' then else if loadpal(p.files[1],'.pal')=-1 then
 begin
  reportline; exit;
 end else convertpalette(b,pal);
if FMEWrite(fout,b,fd)=-1 then begin reportline; exit; end;
error:=false;
end;
begin
 error:=true;
 a;
 close(f); close(fout);
 if errorcode<>0 then;
 if error then fdelete(fout);
 skip2next(t);
end;

Procedure lfd(var t:text;var p:pmrecord);
var f:file;
 ind:TLFDEntry; cpos:longint;
 tg:ext;
procedure a;
var n,i:integer;
begin
val(p.files[2],n,i);
if i<>0 then begin Makeerror('Invalid number: '+p.files[2]); exit; end;
if openwrite(fout,p.files[1],'.lfd')=-1 then begin reportline; exit; end;
writeln('Writing ',getname(fout));
ind.tag:='RMAP'; ind.name:='resource'; ind.size:=n*sizeof(tlfdentry);
fwrite(fout,ind,sizeof(ind));
seek(fout,sizeof(ind)*(n+1));
for i:=0 to n-1 do
begin
 getline(t,s,true); parse(s,p);
 if length(p.files[1])>4 then begin makeerror('Invalid LFD tag:'+tg); exit; end;
 tg:=p.files[1];
 if tg='' then tg:=ext2tag(getext(p.files[0]));
 if tg='' then begin makeerror('Unrecognized extension: '+getext(p.files[0])+' you must supply a tag'); exit; end;
 if length(tg)<>4 then begin makeerror('Invalid tag:'+tg); exit; end;
 if openread(f,p.files[0],'')=-1 then begin reportline; exit; end;
 move(tg[1],ind.tag,4);
 get8name(p.files[0],s);
 move(s[1],ind.name,8);
 ind.size:=filesize(f);
 fwrite(fout,ind,sizeof(ind));
 write('Adding ',getname(f));
 copyfile(f,fout,filesize(f),heapptr,memavail);
 writeln;
 cpos:=filepos(fout);
 seek(fout,(i+1)*sizeof(ind));
 fwrite(fout,ind,sizeof(ind));
 seek(fout,cpos);
 close(f);
end;
 if errorcode<>0 then Fileerror('Write fault',fout) else error:=false;
end;
begin
 error:=true;
 a;
 close(f); close(fout);
 if error then fdelete(fout);
 if errorcode<>0 then;
 skip2next(t);
end;

procedure Delt(var t:text;var p:pmrecord);
var f:file;
procedure a;
begin
if openwrite(fout,p.files[1],'.dlt')=-1 then begin reportline; exit; end;
getline(t,s,true); parse(s,pm);
 use32:=true;
 use0:=true; lfd_use_0:=pos('S',p.sw)=0;
if openread(f,p.files[0],'.bmp')=-1 then begin reportline; exit; end;
if loadbitmap(f,b)=-1 then begin reportline; exit; end;
if p.files[1]='' then else if loadpal(p.files[1],'.plt')=-1 then
 begin
  reportline; exit;
 end else convertpalette(b,pal);
Writeln('Writing ',getname(fout));
if delt_Write(fout,b)=-1 then reportline else error:=false;
end;
begin
error:=true;
 a;
 close(f); close(fout);
 if errorcode<>0 then;
 if error then fdelete(fout);
 skip2next(t);
end;

Procedure anim(var t:text;var p:pmrecord);
var f:file;
 ah:anim_head; spos,cpos:longint;
 dh:anim_dhead;
procedure a;
var n,i:integer;
begin
val(p.files[2],n,i);
if i<>0 then begin Makeerror('Invalid number: '+p.files[2]); exit; end;
if openwrite(fout,p.files[1],'.anm')=-1 then begin reportline; exit; end;
writeln('Writing ',getname(fout));
ah.ndelts:=n;
fwrite(fout,ah,sizeof(ah));
for i:=0 to n-1 do
begin
 getline(t,s,true); parse(s,p);
 use32:=true;
 use0:=true;
 lfd_use_0:=pos('S',p.sw)=0;
 spos:=filepos(fout);
 fwrite(fout,dh,sizeof(dh));
if openread(f,p.files[0],'.bmp')=-1 then begin reportline; exit; end;
Write('Adding ',getname(f));
if loadbitmap(f,b)=-1 then begin reportline; exit; end;
 close(f);
writeln;
if p.files[1]='' then else if loadpal(p.files[1],'.plt')=-1 then
 begin
  reportline; exit;
 end else convertpalette(b,pal);
if delt_Write(fout,b)=-1 then begin reportline; exit; end;
 dh.delt_size:=filepos(fout)-spos-sizeof(dh);
 cpos:=filepos(fout);
 seek(fout,spos);
 fwrite(fout,dh,sizeof(dh));
 seek(fout,cpos);
end;
 if errorcode<>0 then Fileerror('Write fault',fout) else error:=false;
end;
begin
 error:=true;
 a;
 close(fout);
 if errorcode<>0 then;
 if error then fdelete(fout);
 skip2next(t);
end;

Procedure film(var t:text;var p:pmrecord);
var	fh:film_head;
	fb:film_block;
	fc:film_command;
	i,j,l,n:integer;
	fh_offs,fb_offs,cpos:longint;
	b:word;
	out:boolean;
	pc:pcmd_desc;
	prms:cmd_params;
	last_cmd:boolean;
procedure a;
begin
if openwrite(f,p.files[1],'.flm')=-1 then exit;
Write('Writing ',getname(f));
getline(t,s,true); parse(s,p);
fh_offs:=filepos(f);
fh.four:=4;fh.nblocks:=0;
i:=convertstr(p.files[0],fh.duration);
if i=bad_value then begin makeerror('Bad value: '+p.files[0]); exit; end;
fh.duration:=i;
fwrite(f,fh,sizeof(fh));
repeat
getline(t,s,true); parse(s,p);
if p.files[0]='' then break;
pc:=get_cmd_byname(p.files[0]);
if pc=nil then begin makeerror('Unknown keyword '+p.files[0]); exit; end;
if pc^.cmd>0 then begin makeerror('Block identifier expected'); exit; end;
fillchar(fb,sizeof(fb),0); move(pc^.name[2],fb.typename,4);
if length(p.files[1])>8 then begin makeerror('Filename must be <=8 characters'); exit; end;
slocase(p.files[1]); move(p.files[1][1],fb.filename,length(p.files[1]));
fb.fullsize:=22; fb.blocktype:=-pc^.cmd; fb.ncommands:=0; fb.datasize:=0;
if fb.blocktype=bt_end then break;
fb_offs:=filepos(f); fwrite(f,fb,sizeof(fb));
last_cmd:=false;
repeat
minus:=false;
getline(t,s,true); parse(s,p);
minus:=true;
if p.files[0][1]='"' then
begin
 pc:=nil;
 delete(p.files[0],1,1); delete(p.files[0],length(p.files[0]),1);
 val(p.files[0],i,b); if b<>0 then begin makeerror('Invalid number: '+p.files[0]); exit; end;
end
else
begin
 pc:=get_cmd_byname(p.files[0]);
 if pc=nil then begin makeerror('Unknown keyword '+p.files[0]); exit; end;
 if pc^.cmd<0 then begin makeerror('Every block must end with END command'); exit; end;
end;
if get_cmd_params(s,prms,fh.duration)=-1 then begin makeerror('Unrecognized parameters'); exit; end;
if pc<>nil then if (prms.n<>pc^.n1) and (prms.n<>pc^.n2) then begin makeerror('Wrong number of parameters'); exit; end;
if pc=nil then fc.h.command:=i else fc.h.command:=pc^.cmd;
 fc.h.cmd_size:=sizeof(fc.h)+prms.n*2;
 move(prms.p,fc.params,prms.n*2);
 last_cmd:=fc.h.command=cm_end;
 fwrite(f,fc,fc.h.cmd_size);
 inc(fb.datasize,fc.h.cmd_size);
 inc(fb.fullsize,fc.h.cmd_size);
 inc(fb.ncommands);
until last_cmd;
cpos:=filepos(f);
seek(f,fb_offs); fwrite(f,fb,sizeof(fb)); seek(f,cpos);
inc(fh.nblocks);
until false;
fb.fullsize:=18;
if fb.blocktype<>bt_end then
begin
 fb.typename:='END'#0;
 fb.filename:='UNTITLED';
 fb.blocktype:=bt_end;
 fb.fullsize:=18;
end;
fwrite(f,fb,fb.fullsize);
cpos:=filepos(f); seek(f,fh_offs); fwrite(f,fh,sizeof(fh)); seek(f,cpos);
if errorcode<>0 then exit;
error:=false;
end;

begin
 error:=true;
 a;
 close(fout);
 if errorcode<>0 then;
 if error then fdelete(fout);
 skip2next(t);
end;

procedure Animated(var t:text;var p:pmrecord);
var bd:bm_details; n,i,fr:integer;
    m:bm_magic;
    bh:bm_header;
    bm:bm_multi;
    bmh:bm_mheader;
    l,tpos,cpos,spos,bpos:longint;
procedure a;
var i:integer;
begin
 if p.files[2]='' then begin Makeerror('Empty number of frames '); end;
 val(p.files[2],n,i);
 if (i<>0) or (i>255) then begin makeerror('Bad number of frames: '+p.files[2]); exit; end;
 if p.files[3]='' then fr:=5 else val(p.files[3],fr,i);
 if openwrite(fout,p.files[1],'.bm')=-1 then begin reportline; exit; end;
 Write('Writing ',getname(fout),':');
 if fr=0 then writeln(' Switch') else writeln(' Animated, ',n,' frames, framerate=',fr);
 bmwritemagic(fout);
 fillchar(bh,sizeof(bh),0);
 bh.sizex:=1;
 bh.idemx:=$FFFE;
 bh.idemy:=n;
 bh.transparent:=$36;
 fwrite(fout,bh,sizeof(bh));
 bm.speed:=fr;
 bm.two:=2;
 fwrite(fout,bm,sizeof(bm));
 tpos:=filepos(fout);
 seek(fout,tpos+n*4);
 for i:=1 to n do
 begin
  cpos:=filepos(fout);
  getline(t,s,true);
  parse(s,p); getbd(p,bd);
  if bd.compressed<>0 then begin makeerror('Multiple BM cannot be compresed'); bd.compressed:=0; end;
  use32:=pos('L',p.sw)<>0;
  use0:=pos('S',p.sw)=0;
  if openread(f,p.files[0],'.bmp')=-1 then begin reportline; exit; end;
  if loadbitmap(f,b)=-1 then begin reportline; exit; end;
  close(f);
  if p.files[1]='' then else if loadpal(p.files[1],'.pal')=-1 then
  begin
   reportline; exit;
  end else convertpalette(b,pal);
  bmfillmheader(bmh,b,bd);
  fwrite(fout,bmh,sizeof(bmh));
  bmwritedata(fout,b,bd,true);
  spos:=filepos(fout);
  seek(fout,tpos+(i-1)*4);
  bpos:=cpos-$22;
  fwrite(fout,bpos,sizeof(cpos));
  seek(fout,spos);
 end;
 seek(fout,sizeof(bm_magic));
 bh.sizey:=spos-$20;
 fwrite(fout,bh,sizeof(bh));
 if spos>65536 then Writeln('Warning! Multiple BM >64Kb - probably won''t work');
 if errorcode=0 then error:=false;
end;

begin
error:=true;
a;
 close(f); close(fout);
 if errorcode<>0 then;
 if error then fdelete(fout);
 skip2next(t);
end;

procedure Switch(var t:text;var p:pmrecord);
begin
 if p.files[2]='' then p.files[2]:='2';
 p.files[3]:='0';
 animated(t,p);
end;

procedure WAXmessage(var t:text;var p:pmrecord);
begin
 Makeerror('For making WAX files, use 2WAX');
 skip2next(t);
end;



procedure MakeFME(p:pmrecord);
var fd:fme_details;
begin
getfd(p,fd);
if openread(f,p.files[0],'.bmp')=-1 then exit;
if loadbitmap(f,b)=-1 then exit;
if p.files[2]='' then else if loadpal(p.files[2],'.pal')=-1 then exit
 else convertpalette(b,pal);
fd.xshift:=-(b.w shr 1);
fd.yshift:=-b.h;
if openwrite(fout,p.files[1],'.fme')=-1 then exit;
if fmewrite(fout,b,fd)=-1 then fdelete(fout);
end;

procedure MakeBM(p:pmrecord);
var bd:bm_details;
begin
getbd(p,bd);
if openread(f,p.files[0],'.bmp')=-1 then exit;
if loadbitmap(f,b)=-1 then exit;
if p.files[2]='' then else if loadpal(p.files[2],'.pal')=-1 then exit
 else convertpalette(b,pal);
if openwrite(fout,p.files[1],'.bm')=-1 then exit;
if BMwrite(fout,b,bd)=-1 then fdelete(fout);
end;

procedure MakeDELT(p:pmrecord);
begin
use32:=true;
 use0:=true; lfd_use_0:=pos('S',p.sw)=0;
if openread(f,p.files[0],'.bmp')=-1 then exit;
if loadbitmap(f,b)=-1 then exit;
if p.files[2]='' then else if loadpal(p.files[2],'.plt')=-1 then exit
 else convertpalette(b,pal);
if openwrite(fout,p.files[1],'.dlt')=-1 then exit;
Writeln('Writing ',getname(fout));
if Delt_write(fout,b)=-1 then fdelete(fout);
end;


procedure make(n:string);
var t:text;
begin
assign(t,n);
{$i-}
reset(t);
if errorcode<>0 then begin Writeln('Can''t open makefile ',n); exit; end;
Repeat
 getline(t,s,true);
 minus:=false;
 parse(s,pm);
 minus:=true;
 w:=pm.files[0];
 if w='' then continue else
 if w='SIMPLE' then simple(t,pm) else
 if w='FRAME' then frame(t,pm) else
 if w='ANIMATED' then animated(t,pm) else
 if w='SWITCH' then switch(t,pm) else
 if w='MESSAGE' then message(t,pm) else
 if w='SET' then setvars(t,pm) else
 if w='DELT' then delt(t,pm) else
 if w='ANIM' then anim(t,pm) else
 if w='FILM' then film(t,pm) else
 if w='WAX' then waxmessage(t,pm) else
 if w='LFD' then lfd(t,pm) else writeln('Unrecongnized keyword: ',w);
Until eof(t);
end;

Procedure Help(s:string);
begin
 if s<>'' then writeln(s);
 Writeln('2DF - converts Windows 256 color BMP or PCX into diffrenet Dark Forces'#13#10+
	 'graphics file format'#13#10+
	 '2DF [switches] [file1] [file2] [file3]'#13#10+
	 'File1 - BMP file, File2 - FME or BM file. File2 - PAL file'#13#10,
	 'If you omit File3 (PAL file), no palette conversion is performed');
 Writeln('Switches:'#13#10+
	 '/U - uncompressed (FME).');
 Writeln('/F - force FME output'#13#10+
	 '/D - force DELT (DLT) output'#13#10+
	 '/B - force BM output'#13#10+
	 '/X - force PCX input'#13#10+
	 '/T - transparent (BM)'#13#10);
 Writeln('/M - File1 is a makefile (see BMPDF.TXT and MAKEFILE.TXT)'#13#10+
	 '/L - allow to use light-in-the-dark colors in palette conversion'#13#10+
	 '/0=# - set color # a background (transparent) color in DELT and ANIM'#13#10+
	 '/S - solid bitmap - no transparent areas');
 Writeln('Examples:'#13#10,
	 ' 2df /t bars.bmp bars.bm'#13#10,
	 ' 2df /f vader01.bmp vader01.fme /l'#13#10,
	 ' 2df /f/u redkey.bmp redkey.fme'#13#10,
	 ' 2df /d brief01.bmp secbase.dlt brf-jan.plt'#13#10+
	 ' 2df /m switches.txt');
 halt;
end;


procedure getparams;
begin
 parse(string(ptr(PrefixSeg,$80)^),pm);
 if (pm.files[0]='') then help('');
end;

procedure set_lfd_0(var p:pmrecord);
var s:^string; w:string[20]; p0:byte;a,v:word;
begin
p0:=pos('0',pm.sw); if p0=0 then exit;
s:=ptr(PrefixSeg,$80);
getword(s^,pm.swpos[p0],w);
if (length(w)<4) or (w[3]<>'=') then begin Writeln('Invalid /0=# switch');exit; end;
delete(w,1,3);val(w,v,a);
if a<>0 then begin writeln('Invalid # of transparent color: ',w); exit; end;
if v>255 then begin writeln('# of transparent color must be <=255'); exit; end;
lfd_0:=v;
end;


var i,n:integer;
d:bm_details;
 action:(mak,fme,bm,dlt,def);
 oext:ext;
begin
filemode:=0;
getparams;
smartguess(pm);
action:=def;

 oext:=getext(pm.files[0]);
 slocase(oext);
 if (oext='.txt') or (oext='.mak') then action:=mak;

 oext:=getext(pm.files[1]);
 slocase(oext);
 if oext='.fme' then action:=fme;
 if oext='.bm' then action:=bm;
 if oext='.dlt' then action:=dlt;

if pos('0',pm.sw)<>0 then set_lfd_0(pm);
if pos('L',pm.sw)<>0 then use32:=true;
if pos('S',pm.sw)<>0 then use0:=false;

if pos('M',pm.sw)<>0 then action:=mak;
if pos('F',pm.sw)<>0 then action:=fme;
if pos('D',pm.sw)<>0 then action:=dlt;
if pos('B',pm.sw)<>0 then action:=bm;
if pos('X',pm.sw)<>0 then pic_type:=2;


 case action of
	mak:make(pm.files[0]);
	fme:makefme(pm);
	dlt:makedelt(pm);
       else makeBM(pm);
 end;
end.