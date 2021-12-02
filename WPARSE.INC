const minus:boolean=true;
Function openread(var f:file;s:string;dex:ext):integer;
begin
 openread:=0;
if s='' then
 begin
  write('Error: Empty filename ');
  openread:=-1; exit;
 end;
if (s[0]<>#1) and (not (s[1] in ['.','\'])) and (not (s[2]=':')) then s:=idir+s;
if pos('.',s)=0 then s:=s+dex;
assign(f,s);
reset(f,1);
if ioresult<>0 then
 begin
  write('Error opening file ',s); openread:=-1;
 end;
end;

Function openwrite(var f:file;s:string;dex:ext):integer;
begin
 openwrite:=0;
if s='' then
 begin
  write('Error: Empty filename ');
  openwrite:=-1; exit;
 end;
if (s[0]<>#1) and (not (s[1] in ['.','\'])) and (not (s[2]=':')) then s:=odir+s;
if pos('.',s)=0 then s:=s+dex;
assign(f,s);
rewrite(f,1);
if ioresult<>0 then
 begin
  write('Error creating file ',s); openwrite:=-1;
 end;
end;

function locase(c:char):char;
begin
 if c in ['A'..'Z'] then inc(c,32);
 locase:=c;
end;

procedure slocase(var s:string);
var i:integer;
begin
 for i:=1 to length(s) do s[i]:=locase(s[i]);
end;

procedure addslash(var s:fname);
begin
   if (s<>'') and not (s[length(s)] in [':','\']) then s:=concat(s,'\');
end;


procedure get8name(s:string; var n:name8);
var p:byte;i:byte; t:string[80];
begin
fillchar(n,sizeof(n),0);
fillchar(t,sizeof(t),0);
p:=pos('.',s);i:=0;
if p=0 then p:=length(s) else dec(p);
while (p>0) and (s[p]<>'\') and (s[p]<>':') do
 begin insert(locase(s[p]),t,1); dec(p); end;
n:=t;
end;

Function namen(templ:fname;e:ext;n:word):fname;
function num(n:byte):ext;
var s:ext;
begin
 str(n:3,s);
 if s[0]>#3 then s[0]:=#3;
 if s[1]=' ' then s[1]:='0';
 if s[2]=' ' then s[2]:='0';
 num:=s;
end;
begin
 namen:=templ+num(n)+e;
end;

procedure getpath(s:fname;var p:fname);
var i:byte;
begin
i:=length(s);
while (i>0) and not (s[i] in ['\',':']) do dec(i);
p:=copy(s,1,i);
end;

function getext(n:fname):ext;
var p,ep:byte;e:ext;
begin
 if length(n)<4 then ep:=0 else ep:=length(n)-4;
 e:='';p:=length(n);
 while p>ep do begin insert(n[p],e,1); if n[p]='.' then break; dec(p); end;
 if (e='') or (e[1]<>'.') then e:='';
 getext:=e;
end;

function get13name(n:string):fname;
var s:name8;
begin
 get8name(n,s);
 get13name:=s+getext(n);
end;

function gettempl(n:fname):fname;
var n8:name8;dir:fname;
begin
  get8name(n,n8);
  if n8[0]>#5 then n8[0]:=#5;
  getpath(n,dir);
  gettempl:=dir+n8;
end;

function removeext(n:fname):fname;
var p:byte;
begin
 p:=pos('.',n);
 if p=0 then removeext:=n else removeext:=copy(n,1,p-1);
end;

procedure prepare(var s:string);
var i:integer;l:byte;
begin
 l:=pos('#',s);
 if l<>0 then s[0]:=chr(l-1);
 for i:=1 to length(s) do s[i]:=upcase(s[i]);
end;

function getword(var p:string; pos:byte; var s:string):byte;
var i:integer;
begin
 i:=pos; s:=''; getword:=0;
 while (p[i] in [' ',#9]) and (i<=length(p)) do inc(i);
 if i>length(p) then exit;
 s:=p[i]; inc(i);
 while not (p[i] in [' ',#9,'/']) and (i<=length(p)) do begin s:=concat(s,p[i]); inc(i) end;
 getword:=i;
end;

Procedure parse(var s:string;var pm:pmrecord);
var i,n:byte;w:string;
begin
 prepare(s);
 fillchar(pm,sizeof(pm),0);
 i:=1;n:=0;
 repeat
  i:=getword(s,i,w);
  if w='' then break;
  case w[1] of
   '/': pm.sw:=pm.sw+w[2];
   '-': if minus then pm.sw:=pm.sw+w[2] else begin if n<=3 then pm.files[n]:=w; inc(n); end;
  else
    begin
     if n<=3 then pm.files[n]:=w;
     inc(n);
    end;
  end;
 until (w='') or (i>length(s));
end;

Function solve(var s:string; var x,y:string):integer; {returns x and y form "x=y" string}
var p:byte;
begin
 solve:=-1;
 p:=pos('=',s);
 if p=0 then exit;
 x:=copy(s,1,p-1);
 y:=copy(s,p+1,length(s)-p);
 solve:=0;
end;

procedure reportline;
begin
 writeln(' line: ',nline);
end;

procedure getline(var t:text;var s:string;process:boolean);
begin
 readln(t,s);
 inc(nline);
 if process then prepare(s);
end;

procedure skip2next(var t:text);
begin
 repeat
  getline(t,s,false);
 until s='';
end;

Function getident(s:ident):ident_code;
type
	i_rec=record i:ident; c:ident_code end;
const	i_table:array[0..32] of i_rec=(
(i:'CELL';c:C_CELL),
(i:'PAL';c:C_PAL),
(i:'BMP';c:C_BMP),
(i:'PCX';c:C_PCX),
(i:'C';c:C_COMPRESSED),
(i:'SEQ8';c:C_SEQ8),
(i:'SEQ32';c:C_SEQ32),
(i:'FR';c:C_FR),
(i:'WW';c:C_WW),
(i:'WH';c:C_WH),
(i:'FRAMES';c:C_FRAMES),
(i:'CELL';c:C_CELL),
(i:'X';c:C_XSHIFT),
(i:'Y';c:C_YSHIFT),
(i:'FLIP';c:C_FLIP),
(i:'SEQS';c:C_SEQS),
(i:'CELLS';c:C_CELLS),
(i:'WAXES';c:C_WAXES),
(i:'SET';c:C_SET),
(i:'INDIR';c:C_IDIR),
(i:'OUTDIR';c:C_ODIR),
(i:'PALDIR';c:C_PDIR),
(i:'';c:C_EMPTY),
(i:'WAX';c:C_WAX),
(i:'MESSAGE';c:C_MESSAGE),
(i:'SIMPLE';c:C_SIMPLE),
(i:'SWITCH';c:C_SWITCH),
(i:'FRAME';c:C_FRAME),
(i:'ANIMATED';c:C_ANIMATED),
(i:'ANIM';c:C_ANIM),
(i:'DELT';c:C_DELT),
(i:'FILM';c:C_FILM),
(i:'LFD';c:C_LFD));
var i:integer;
begin
 getident:=c_wrong;
 for i:=0 to sizeof(i_table) div sizeof(i_rec)-1 do
  if i_table[i].i=s then begin getident:=i_table[i].c; break; end;
end;
