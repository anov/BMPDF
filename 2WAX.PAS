{$A+,B-,D+,E-,F-,G-,I-,L+,N-,O-,P-,Q-,R-,S-,T-,V-,X+}
{$M 65000,0,655360}

const
	nline:word=0;
	maxheight=6000;
	idir:string[68]='';
	odir:string[68]='';
	pdir:string[68]='';
	use0:boolean=true;
	use32:boolean=false;
{$i wtypes.inc}
const
dpal:tpal=
((red:0;green:0;blue:0),
(red:252;green:252;blue:252),
(red:208;green:236;blue:252),
(red:168;green:220;blue:252),
(red:124;green:204;blue:252),
(red:84;green:192;blue:252),
(red:252;green:0;blue:0),
(red:204;green:0;blue:0),
(red:144;green:0;blue:0),
(red:68;green:0;blue:0),
(red:0;green:252;blue:0),
(red:0;green:200;blue:0),
(red:0;green:152;blue:0),
(red:0;green:92;blue:0),
(red:0;green:52;blue:0),
(red:0;green:88;blue:252),
(red:0;green:36;blue:240),
(red:0;green:16;blue:188),
(red:0;green:4;blue:140),
(red:248;green:224;blue:96),
(red:244;green:184;blue:52),
(red:244;green:136;blue:12),
(red:216;green:88;blue:12),
(red:180;green:44;blue:4),
(red:252;green:0;blue:252),
(red:252;green:0;blue:252),
(red:252;green:0;blue:252),
(red:252;green:0;blue:252),
(red:252;green:0;blue:252),
(red:252;green:0;blue:252),
(red:252;green:0;blue:252),
(red:252;green:0;blue:252),
(red:232;green:232;blue:232),
(red:224;green:224;blue:224),
(red:216;green:216;blue:216),
(red:208;green:208;blue:208),
(red:204;green:204;blue:204),
(red:196;green:196;blue:196),
(red:188;green:188;blue:188),
(red:180;green:180;blue:180),
(red:172;green:172;blue:172),
(red:168;green:168;blue:168),
(red:160;green:160;blue:160),
(red:152;green:152;blue:152),
(red:144;green:144;blue:144),
(red:140;green:140;blue:140),
(red:132;green:132;blue:132),
(red:124;green:124;blue:124),
(red:116;green:116;blue:116),
(red:112;green:112;blue:112),
(red:104;green:104;blue:104),
(red:100;green:100;blue:100),
(red:92;green:92;blue:92),
(red:88;green:88;blue:88),
(red:80;green:80;blue:80),
(red:76;green:76;blue:76),
(red:68;green:68;blue:68),
(red:64;green:64;blue:64),
(red:56;green:56;blue:56),
(red:52;green:52;blue:52),
(red:44;green:44;blue:44),
(red:40;green:40;blue:40),
(red:32;green:32;blue:32),
(red:28;green:28;blue:28),
(red:100;green:108;blue:132),
(red:92;green:100;blue:120),
(red:84;green:92;blue:112),
(red:76;green:84;blue:104),
(red:72;green:76;blue:96),
(red:64;green:68;blue:84),
(red:56;green:64;blue:76),
(red:52;green:56;blue:68),
(red:44;green:48;blue:60),
(red:36;green:40;blue:48),
(red:28;green:32;blue:40),
(red:24;green:24;blue:32),
(red:16;green:16;blue:24),
(red:8;green:12;blue:12),
(red:4;green:4;blue:4),
(red:0;green:0;blue:0),
(red:252;green:228;blue:176),
(red:236;green:208;blue:152),
(red:220;green:192;blue:132),
(red:208;green:176;blue:116),
(red:192;green:160;blue:96),
(red:180;green:144;blue:80),
(red:164;green:132;blue:68),
(red:148;green:116;blue:52),
(red:136;green:100;blue:40),
(red:120;green:88;blue:28),
(red:108;green:76;blue:20),
(red:92;green:64;blue:12),
(red:76;green:52;blue:8),
(red:64;green:40;blue:4),
(red:48;green:28;blue:0),
(red:36;green:20;blue:0),
(red:224;green:108;blue:12),
(red:212;green:100;blue:8),
(red:200;green:92;blue:8),
(red:188;green:88;blue:4),
(red:176;green:80;blue:4),
(red:168;green:72;blue:4),
(red:156;green:68;blue:4),
(red:144;green:60;blue:4),
(red:132;green:56;blue:0),
(red:120;green:52;blue:0),
(red:112;green:44;blue:0),
(red:100;green:40;blue:0),
(red:88;green:32;blue:0),
(red:76;green:28;blue:0),
(red:64;green:24;blue:0),
(red:56;green:20;blue:0),
(red:128;green:228;blue:100),
(red:112;green:212;blue:84),
(red:100;green:200;blue:68),
(red:92;green:188;blue:56),
(red:80;green:176;blue:44),
(red:68;green:164;blue:32),
(red:60;green:148;blue:24),
(red:52;green:136;blue:12),
(red:44;green:124;blue:8),
(red:36;green:112;blue:0),
(red:32;green:100;blue:0),
(red:48;green:84;blue:4),
(red:56;green:72;blue:4),
(red:56;green:56;blue:8),
(red:44;green:40;blue:8),
(red:32;green:24;blue:8),
(red:252;green:212;blue:200),
(red:244;green:196;blue:180),
(red:240;green:184;blue:164),
(red:232;green:172;blue:148),
(red:224;green:160;blue:132),
(red:220;green:148;blue:116),
(red:212;green:136;blue:104),
(red:208;green:128;blue:92),
(red:200;green:116;blue:76),
(red:196;green:108;blue:64),
(red:188;green:100;blue:52),
(red:184;green:92;blue:40),
(red:176;green:84;blue:32),
(red:172;green:80;blue:20),
(red:164;green:72;blue:12),
(red:160;green:68;blue:4),
(red:148;green:92;blue:0),
(red:136;green:76;blue:0),
(red:124;green:60;blue:0),
(red:112;green:48;blue:0),
(red:104;green:36;blue:0),
(red:92;green:28;blue:0),
(red:80;green:16;blue:0),
(red:72;green:12;blue:0),
(red:0;green:0;blue:252),
(red:0;green:0;blue:224),
(red:0;green:0;blue:200),
(red:0;green:0;blue:176),
(red:0;green:0;blue:148),
(red:0;green:0;blue:124),
(red:0;green:0;blue:100),
(red:0;green:0;blue:76),
(red:252;green:0;blue:0),
(red:224;green:0;blue:0),
(red:196;green:0;blue:0),
(red:168;green:0;blue:0),
(red:140;green:0;blue:0),
(red:112;green:0;blue:0),
(red:84;green:0;blue:0),
(red:56;green:0;blue:0),
(red:252;green:128;blue:0),
(red:224;green:108;blue:0),
(red:200;green:92;blue:0),
(red:176;green:76;blue:0),
(red:152;green:60;blue:0),
(red:128;green:44;blue:0),
(red:104;green:32;blue:0),
(red:80;green:24;blue:0),
(red:192;green:112;blue:68),
(red:180;green:104;blue:60),
(red:168;green:96;blue:56),
(red:156;green:88;blue:52),
(red:148;green:84;blue:48),
(red:136;green:76;blue:44),
(red:124;green:68;blue:36),
(red:116;green:64;blue:32),
(red:104;green:56;blue:28),
(red:92;green:48;blue:24),
(red:80;green:44;blue:20),
(red:72;green:36;blue:16),
(red:60;green:32;blue:12),
(red:48;green:24;blue:8),
(red:40;green:20;blue:8),
(red:60;green:32;blue:16),
(red:184;green:232;blue:252),
(red:168;green:224;blue:248),
(red:156;green:220;blue:248),
(red:140;green:212;blue:244),
(red:128;green:204;blue:240),
(red:116;green:196;blue:240),
(red:100;green:192;blue:236),
(red:88;green:184;blue:236),
(red:76;green:176;blue:232),
(red:64;green:172;blue:232),
(red:52;green:164;blue:228),
(red:40;green:156;blue:228),
(red:28;green:148;blue:224),
(red:16;green:140;blue:224),
(red:4;green:136;blue:220),
(red:0;green:128;blue:220),
(red:100;green:108;blue:132),
(red:96;green:104;blue:128),
(red:92;green:100;blue:120),
(red:88;green:96;blue:116),
(red:84;green:92;blue:112),
(red:80;green:88;blue:108),
(red:80;green:84;blue:104),
(red:76;green:80;blue:100),
(red:72;green:76;blue:96),
(red:68;green:76;blue:92),
(red:64;green:72;blue:88),
(red:60;green:68;blue:84),
(red:60;green:64;blue:80),
(red:56;green:60;blue:72),
(red:52;green:56;blue:68),
(red:48;green:52;blue:64),
(red:44;green:48;blue:60),
(red:40;green:44;blue:56),
(red:40;green:44;blue:52),
(red:36;green:40;blue:48),
(red:32;green:36;blue:44),
(red:28;green:32;blue:40),
(red:24;green:28;blue:36),
(red:24;green:24;blue:32),
(red:20;green:20;blue:24),
(red:16;green:16;blue:20),
(red:12;green:12;blue:16),
(red:8;green:8;blue:12),
(red:4;green:8;blue:8),
(red:4;green:4;blue:4),
(red:0;green:0;blue:0),
(red:0;green:0;blue:0),
(red:0;green:0;blue:144),
(red:0;green:0;blue:132),
(red:0;green:0;blue:120),
(red:0;green:0;blue:112),
(red:0;green:0;blue:100),
(red:0;green:0;blue:88),
(red:0;green:0;blue:80),
(red:0;green:0;blue:68),
(red:0;green:0;blue:60),
(red:0;green:0;blue:48),
(red:0;green:0;blue:36),
(red:0;green:0;blue:28),
(red:0;green:0;blue:16),
(red:0;green:0;blue:4),
(red:0;green:0;blue:0),
(red:252;green:252;blue:252));

var
    pm:pmrecord;
    w,s:string;
    file1,file2,file3:string[80];
    pal:tpal;
    f,fout:file;
    b:tbitmap;
    buf:array[0..maxheight-1] of byte;
    cbuf:array[0..maxheight-1] of byte;
    cols:array[0..maxheight-1] of longint;
    waxes:array[0..255] of wax_data;
    seqs:array[0..255] of seq_data;
    frames:array[0..255] of frame_data;
    cells:array[0..255] of cell_data;
    nwaxes,nseqs,nframes,ncells:integer;
    t_seqs,t_frames,t_cells:array[0..255] of byte;
    Cell_offs:array[0..255] of longint;


{$i bmp2wax.inc}
{$i wparse.inc}

procedure makeerror(s:string);
begin
 if s<>'' then write('Syntax: ',s);
 reportline;
end;

procedure makewarn(s:string);
begin
 if s<>'' then write('Warning: ',s);
 reportline;
end;


function loadpal(s:string):integer;
var p:file; i:integer;
begin
loadpal:=-1;
if openread(p,s,'.pal')=-1 then exit;
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

function loadlfdpal(s:string):integer;
var p:file; i:integer;
   pl:lfd_pal;
begin
loadlfdpal:=-1;
if openread(p,s,'.plt')=-1 then exit;
fread(p,pl,2);
if (pl.first<>0) or (pl.last<>255) then begin Fileerror('Not a full pallette',p); close(p); exit; end;
fread(p,pal,sizeof(pal));
close(p);
if errorcode=0 then loadlfdpal:=0 else Fileerror('Read fault',p);
end;

type	lbres=(a_short,a_long,a_badchar,a_ok,a_bigval);

function loadbytearray(var s:string;var b; var size:byte):lbres;
{loads values from string '#,#,#..' to byte array}
label ret;
type
	bytes=array[0..0] of byte;
var	i:integer;
	a,v:word;
	ps:byte;
	vs:string;
begin
loadbytearray:=a_ok; ps:=1;
for i:=0 to size-1 do
 begin
  if ps=length(s)+2 then begin loadbytearray:=a_short; dec(i); goto ret; end;
  vs:='';
  while (ps<=length(s)) and (s[ps]<>',') do begin vs:=vs+s[ps]; inc(ps); end;
  inc(ps);
  val(vs,v,a);
  if a<>0 then begin loadbytearray:=a_badchar; goto ret end;
  if v>254 then begin loadbytearray:=a_bigval; goto ret end;
  bytes(b)[i]:=v;
 end;
if ps<length(s)+2 then loadbytearray:=a_long;
ret:
   size:=i+1;
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
 case getident(p.files[1]) of
  C_IDIR:begin
	  idir:=p.files[2];
	  addslash(idir);
	 end;
  C_ODIR:begin
	  odir:=p.files[2];
	  addslash(odir);
	 end;
  C_PDIR:begin
	  pdir:=p.files[2];
	  addslash(pdir);
	 end;
 else makeerror('Invalid SET statement ');
 end;
end;

Procedure wrongword(var t:text; var p:pmrecord);
begin
 Writeln('Unrecognized keyword ',p.files[0]); reportline;
 skip2next(t);
end;

Procedure bmp2df(var t:text; var p:pmrecord);
begin
 Write('Use 2DF to process ',p.files[0],' sequence.'); reportline;
 skip2next(t);
end;

Function loadwaxes(var t:text; var p:pmrecord):integer;
var	i,j:integer;a,v:word;
	ps:byte; tag:byte;
	seq_set:boolean;
	var x,y:string;
	n:byte;
	sq8:array[0..7] of byte;
	l:longint;
begin
loadwaxes:=-1;
fillchar(waxes,sizeof(waxes),255); nwaxes:=0;
val(p.files[1],v,a);
if a<>0 then begin Makeerror('Wrong number of WAXES: '+p.files[1]); exit; end;
if a>255 then begin Makeerror('too many WAXES. limit - 255'); exit; end;
nwaxes:=v;
for i:=0 to nwaxes-1 do
begin
getline(t,s,true);ps:=1;
ps:=getword(s,ps,w);
if (w='') or (w[length(w)]<>':') then begin makeerror('The WAXES entry must begin with #:'); exit; end;
seq_set:=false;
 waxes[i].ww:=65536;
 waxes[i].wh:=65536;
 waxes[i].framerate:=8;
repeat
 ps:=getword(s,ps,w);
 if solve(w,x,y)=-1 then begin makeerror('Invalid statement: '+w); exit; end;
 case getident(x) of
  c_seq8: begin
	   n:=8;
	   case loadbytearray(y,sq8,n) of
	    a_long: makewarn('Extra characters in SEQ8=');
	    a_short: begin makeerror('SEQ8= must have 8 entries'); exit; end;
	    a_badchar: begin makeerror('Bad characters in SEQ8='); exit; end;
	    a_bigval: begin makeerror('tags must be <255'); exit; end;
	   end;
	    waxes[i].seqs[0]:=sq8[0];
	    waxes[i].seqs[1]:=sq8[0];
	    waxes[i].seqs[31]:=sq8[0];
	    waxes[i].seqs[30]:=sq8[0];
	    for j:=1 to 7 do
	     begin
	      waxes[i].seqs[(j-1)*4+2]:=sq8[j];
	      waxes[i].seqs[(j-1)*4+3]:=sq8[j];
	      waxes[i].seqs[(j-1)*4+4]:=sq8[j];
	      waxes[i].seqs[(j-1)*4+5]:=sq8[j];
	     end;
	   seq_set:=true;
	  end;
  c_seq32: begin
	    n:=32;
	    case loadbytearray(y,waxes[i].seqs,n) of
	     a_long: makewarn('Extra characters in SEQ32=');
	     a_short: begin makeerror('SEQ32= must have 32 entries'); exit; end;
	     a_badchar: begin makeerror('Bad characters in SEQ32='); exit; end;
	     a_bigval: begin makeerror('tags must be <255'); exit; end;
	    end;
	    seq_set:=true;
	   end;
  c_ww: begin
	 val(y,l,a);
	 if a<>0 then makeerror('Invalid number in ww=')
	 else waxes[i].ww:=l;
	end;
  c_wh: begin
	 val(y,l,a);
	 if a<>0 then makeerror('Invalid number in wh=')
	 else waxes[i].wh:=l;
	end;
  c_fr: begin
	 val(y,l,a);
	 if a<>0 then makeerror('Invalid number in ww=')
	 else waxes[i].framerate:=l;
	end;
 else begin makeerror('Invalid field: '+x); exit; end;
 end;
until (w='') or (ps>length(s));
if not seq_set then begin makeerror('No SEQ#= encountered'); exit; end;
end;
loadwaxes:=0;
end;

Function loadseqs(var t:text; var p:pmrecord):integer;
var	i,j:integer;a,v:word;
	ps:byte; tag:byte;
	var x,y:string;
	n:byte;
begin
loadseqs:=-1;
fillchar(seqs,sizeof(seqs),255); nseqs:=0;
val(p.files[1],v,a);
if a<>0 then begin Makeerror('Wrong number of SEQS: '+p.files[1]); exit; end;
if a>255 then begin Makeerror('too many SEQS. limit - 255'); exit; end;
nseqs:=v;
fillchar(t_seqs,sizeof(t_seqs),255);
for i:=0 to nseqs-1 do
begin
getline(t,s,true);ps:=1;
ps:=getword(s,ps,w);
if (w='') or (w[length(w)]<>':') then begin makeerror('The SEQS entry must begin with #:'); exit; end;
dec(w[0]);val(w,v,a);
if a<>0 then begin makeerror('Invalid tag: '+w); exit; end;
if v>254 then begin makeerror('Tag must be in range 0..254'); exit; end;
if t_seqs[v]<>255 then begin makeerror('Duplicate tag: '+w); exit; end;
 t_seqs[v]:=i;
 ps:=getword(s,ps,w);
 if solve(w,x,y)=-1 then begin makeerror('Invalid statement: '+w); exit; end;
 if getident(x)<>c_frames then begin makeerror('Invalid statement: '+w); exit; end;
  n:=32;
  case loadbytearray(y,seqs[i].frames,n) of
   a_long: makewarn('Extra characters in FRAMES=');
   a_badchar: begin makeerror('Bad characters in FRAMES='); exit; end;
   a_bigval: begin makeerror('tags must be <255'); exit; end;
  end;
end;
loadseqs:=0;
end;

Function loadframes(var t:text; var p:pmrecord):integer;
var	i,j:integer;a,v:word;
	ps:byte; tag:byte;
	cell_set:boolean;
	x,y:string;
	n:byte;
	l:longint;
begin
loadframes:=-1;
fillchar(frames,sizeof(frames),255); nframes:=0;
val(p.files[1],v,a);
if a<>0 then begin Makeerror('Wrong number of FRAMES: '+p.files[1]); exit; end;
if a>255 then begin Makeerror('too many FRAMES. limit - 255'); exit; end;
nframes:=v;
fillchar(t_frames,sizeof(t_frames),255);
for i:=0 to nframes-1 do
begin
getline(t,s,true);ps:=1;
ps:=getword(s,ps,w);
if (w='') or (w[length(w)]<>':') then begin makeerror('The FRAMES entry must begin with #:'); exit; end;
dec(w[0]);val(w,v,a);
if a<>0 then begin makeerror('Invalid tag: '+w); exit; end;
if v>254 then begin makeerror('Tag must be in range 0..254'); exit; end;
if t_frames[v]<>255 then begin makeerror('Duplicate tag: '+w); exit; end;
t_frames[v]:=i;
cell_set:=false;
 frames[i].xshift:=-32767;
 frames[i].yshift:=-32767;
 frames[i].flip:=0;
repeat
 ps:=getword(s,ps,w);
 if solve(w,x,y)=-1 then begin makeerror('Invalid statement: '+w); exit; end;
 case getident(x) of
  c_cell: begin
	   val(y,v,a);
	   if a<>0 then begin makeerror('Invalid number in CELL='); exit; end
	   else frames[i].cell:=v;
	   cell_set:=true;
	  end;
  c_xshift: begin
	 val(y,l,a);
	 if a<>0 then makeerror('Invalid number in XSHIFT=')
	 else frames[i].xshift:=l;
	end;
  c_yshift: begin
	 val(y,l,a);
	 if a<>0 then makeerror('Invalid number in YSHIFT=')
	 else frames[i].yshift:=l;
	end;
  c_flip: begin
	 val(y,v,a);
	 if a<>0 then makeerror('Invalid number in FLIP=')
	 else frames[i].flip:=v;
	end;
 else begin makeerror('Invalid field: '+x); exit; end;
 end;
until (w='') or (ps>length(s));
if not cell_set then begin makeerror('No CELL= encountered'); exit; end;
end;
loadframes:=0;
end;

Function Writecells(var t:text; var p:pmrecord):integer;
var	i,j:integer;a,v:word;
	ps:byte; tag:byte;
	pic_set:byte;
	pal_set:boolean;
	x,y:string;
	n:byte;
	l:longint;
	bmpname:fname;
begin
l:=sizeof(wax_header)+nwaxes*sizeof(twax)+nseqs*sizeof(tseq)+nframes*sizeof(fme_header1);
seek(fout,l);
writecells:=-1;
fillchar(cells,sizeof(cells),255); ncells:=0;
val(p.files[1],v,a);
if a<>0 then begin Makeerror('Wrong number of CELLS: '+p.files[1]); exit; end;
if a>255 then begin Makeerror('too many CELLS. limit - 255'); exit; end;
ncells:=v;
fillchar(t_cells,sizeof(t_cells),255);
for i:=0 to ncells-1 do
begin
getline(t,s,true);ps:=1;
ps:=getword(s,ps,w);
if (w='') or (w[length(w)]<>':') then begin makeerror('The CELLS entry must begin with #:'); exit; end;
dec(w[0]);val(w,v,a);
if a<>0 then begin makeerror('Invalid tag: '+w); exit; end;
if v>254 then begin makeerror('Tag must be in range 0..254'); exit; end;
if t_cells[v]<>255 then begin makeerror('Duplicate tag: '+w); exit; end;
t_cells[v]:=i;
 cells[i].w:=0;
 cells[i].h:=0;
 cells[i].compressed:=1;
 use0:=true;
 use32:=false;
 pic_set:=0;
 pal_set:=false;
repeat
 ps:=getword(s,ps,w);
 if solve(w,x,y)=-1 then begin makeerror('Invalid statement: '+w); exit; end;
 case getident(x) of
  c_bmp: begin
	  bmpname:=y;
	  pic_set:=1;
	 end;
  c_pcx: begin
	  bmpname:=y;
	  pic_set:=2;
	 end;
  c_compressed: begin
	 val(y,v,a);
	 if a<>0 then makeerror('Invalid number in C=')
	 else cells[i].compressed:=v;
	end;
  c_pal: begin
	 val(y,v,a);
	 pal_set:=true;
	 if a<>0 then begin makeerror('Invalid number in PAL='); pal_set:=false; end
	 else
	  case v of
	   0: begin use0:=false; use32:=false; end;
	   1: begin use0:=true; use32:=false; end;
	   2: begin use0:=false; use32:=true; end;
	   3: begin use0:=true; use32:=true; end;
	   else begin makewarn('Invalid value in PAL=');  pal_set:=false; end
	  end;
	end;
 else begin makeerror('Invalid field: '+x); exit; end;
 end;
until (w='') or (ps>length(s));
 case pic_set of
  1:begin
     if openread(f,bmpname,'.bmp')=-1 then begin reportline; exit; end;
     if loadBMP(f,b)=-1 then begin reportline; exit; end; close(f);
    end;
  2:begin
     if openread(f,bmpname,'.pcx')=-1 then begin reportline; exit; end;
     if loadPCX(f,b)=-1 then begin reportline; exit; end; close(f);
    end;
  else begin makeerror('No BMP=  or PCX= encountered'); exit; end;
 end;
 cells[i].w:=b.w;
 cells[i].h:=b.h;
 cell_offs[i]:=filepos(fout);
 if pal_set then convertpalette(b,pal);
 Write('Writing ',bmpname,'       '#13);
 if cell_write(fout,b,cells[i])=-1 then begin reportline; exit; end;
end;
writecells:=0;
end;

Function concludewax:integer;
var	wh:wax_header;
	awax:twax;
	aseq:tseq;
	aframe:fme_header1;
	acell:fme_header2;
	i,j:integer;
	waxoffs,seqoffs,frameoffs,celloffs:longint;
	n:byte;
begin
Writeln('Creating references...');
Concludewax:=-1;
waxoffs:=sizeof(wax_header);
seqoffs:=waxoffs+nwaxes*sizeof(twax);
frameoffs:=seqoffs+nseqs*sizeof(tseq);
celloffs:=frameoffs+nframes*sizeof(fme_header1);
seek(fout,0);
fillchar(wh,sizeof(wh),0);
wh.version:=$11000;
wh.nseqs:=nseqs;
wh.nframes:=nframes;
wh.ncells:=ncells;
for i:=0 to nwaxes-1 do wh.waxes[i]:=waxoffs+i*sizeof(twax);
fwrite(fout,wh,sizeof(wh));
seek(fout,waxoffs);
for i:=0 to nwaxes-1 do
begin
 fillchar(awax,sizeof(awax),0);
 awax.wwidth:=waxes[i].ww;
 awax.wheight:=waxes[i].wh;
 awax.Framerate:=waxes[i].framerate;
 for j:=0 to 31 do
 begin
  n:=t_seqs[waxes[i].seqs[j]];
  if n=255 then begin Writeln('No SEQ with tag ',waxes[i].seqs[j]); exit; end;
  awax.seqs[j]:=seqoffs+n*sizeof(tseq);
 end;
 fwrite(fout,awax,sizeof(awax));
end;

seek(fout,seqoffs);
for i:=0 to nseqs-1 do
begin
 fillchar(aseq,sizeof(aseq),0);
 for j:=0 to 31 do
 if seqs[i].frames[j]<>255 then
 begin
  n:=t_frames[seqs[t_seqs[i]].frames[j]];
  if n=255 then begin Writeln('No FRAME with tag ',seqs[t_seqs[i]].frames[j]); exit; end;
  aseq.frames[j]:=frameoffs+n*sizeof(fme_header1);
 end;
 fwrite(fout,aseq,sizeof(aseq));
end;

seek(fout,frameoffs);
for i:=0 to nframes-1 do
begin
 fillchar(aframe,sizeof(aframe),0);
 n:=t_cells[frames[i].cell];
 if n=255 then begin Writeln('No Cell with tag ',frames[i].cell); exit; end;
 if frames[i].xshift=-32767 then aframe.xshift:=-(cells[n].w div 2)
			    else aframe.xshift:=frames[i].xshift;
 if frames[i].yshift=-32767 then aframe.yshift:=-(cells[n].h)
			    else aframe.yshift:=frames[i].yshift;
 aframe.flip:=frames[i].flip;
 aframe.header2:=cell_offs[n];
 fwrite(fout,aframe,sizeof(aframe));
end;
concludewax:=0;
end;

Procedure WAX(var t:text; var p:pmrecord);
Function a:integer;
const
	f_waxes=1;
	f_seqs=2;
	f_frames=4;
	f_cells=8;
var jobs:byte; {job done? bit flags}
    wseq:boolean; {whether seq#= encountered in WAXes sequence}
    sframes:boolean; {whether frames= encountered in SEQS sequence}
    fcell:boolean; {whether cell= encountered in FRAMES sequence) }
    cbmp:boolean; {whether bmp= encountered in CELLS sequence}
    ps:byte; {current position in line)}
    w:ident;
begin
a:=-1;
jobs:=0; pal:=dpal;
if openwrite(fout,p.files[1],'.wax')=-1 then begin reportline; exit; end;
Repeat
getline(t,s,true);
parse(s,p);
case getident(p.files[0]) of
 c_pal: if p.files[1]<>'' then if loadpal(p.files[1])<>-1 then pal:=dpal;
 c_waxes: if (jobs and f_waxes)<>0 then begin makeerror('Duplicate WAXES section'); exit; end
	  else if loadwaxes(t,p)=-1 then exit else jobs:=jobs or f_waxes;
 c_seqs:  if (jobs and f_seqs)<>0 then begin makeerror('Duplicate SEQS section'); exit; end
	  else if loadseqs(t,p)=-1 then exit else jobs:=jobs or f_seqs;
 c_frames:if (jobs and f_frames)<>0 then begin makeerror('Duplicate FRAMES section'); exit; end
	  else if loadframes(t,p)=-1 then exit else jobs:=jobs or f_frames;
 c_cells: if jobs<>f_waxes+f_seqs+f_frames then begin Makeerror('Cells section must be last section in WAX'); exit; end
	  else if writecells(t,p)=-1 then exit else jobs:=jobs or f_cells;
 else begin makeerror('Unrecognized keyword'); exit; end;
end;
 Until jobs=f_waxes+f_seqs+f_frames+f_cells;
if concludewax=-1 then exit;
a:=0;
close(fout);
Writeln('Done.');
end;

begin
 if a=-1 then fdelete(fout);
 skip2next(t);
end;

procedure make(var t:text);
begin
Repeat
 getline(t,s,true);
 minus:=false;
 parse(s,pm);
 minus:=true;
 case getident(pm.files[0]) of
  C_EMPTY:continue;
  C_SET:setvars(t,pm);
  C_WAX:WAX(t,pm);
  C_MESSAGE: message(t,pm);
  C_SIMPLE,C_FRAME,C_ANIMATED,C_SWITCH,C_ANIM,C_DELT,C_LFD,C_FILM:bmp2df(t,pm);
 else wrongword(t,pm);
 end;
Until eof(t);
end;

Procedure Help(s:string);
begin
 if s<>'' then writeln(s);
 Writeln('2WAX - converts  a set of Windows 256 color BMP and PCX files into a WAX.'#13#10+
	 '2WAX makefile');
 Writeln('makefile is the text file containing a description of a WAX file'#13#10+
	 'See BMPDF.TXT for details');
 Writeln('Examples:'#13#10,
	 ' 2WAX reeyees.txt');
 halt;
end;

procedure getparams;
begin
 parse(string(ptr(PrefixSeg,$80)^),pm);
 if (pm.files[0]='') then help('');
end;

var i,n:integer;t:text;
a:ident_code;
s1,s2,s3:string;
r:array[0..100] of byte;
h:byte;
begin
filemode:=0;
getparams;
assign(t,pm.files[0]);
reset(t);
if ioresult<>0 then begin Writeln('Can''t open makefile ',pm.files[0]); halt; end;
make(t);
close(t);
end.