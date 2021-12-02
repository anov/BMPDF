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

Function popenread(var f:file;s:string;dex:ext):integer;
var dir:fname;
begin
 popenread:=0;
if s='' then
 begin
  write('Error: Empty filename ');
  popenread:=-1; exit;
 end;
if pdir='' then dir:=idir else dir:=pdir;
if (s[0]<>#1) and (not (s[1] in ['.','\'])) and (not (s[2]=':')) then s:=dir+s;
if pos('.',s)=0 then s:=s+dex;
assign(f,s);
reset(f,1);
if ioresult<>0 then
 begin
  write('Error opening file ',s); popenread:=-1;
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

function getext(n:fname):ext;
var p,ep:byte;e:ext;
begin
 if length(n)<4 then ep:=0 else ep:=length(n)-4;
 e:='';p:=length(n);
 while p>ep do begin insert(n[p],e,1); if n[p]='.' then break; dec(p); end;
 if (e='') or (e[1]<>'.') then e:='';
 getext:=e;
end;

function removeext(n:fname):fname;
var p:byte;
begin
 p:=pos('.',n);
 if p=0 then removeext:=n else removeext:=copy(n,1,p-1);
end;

function ext2tag(e:ext):ext;
type entry=record
		tag:ext;
		xt:ext;
	       end;
const table:array[1..12] of entry=(
		(tag:'ANIM';xt:'.ANM'),
		(tag:'DELT';xt:'.DLT'),
		(tag:'FILM';xt:'.FLM'),
		(tag:'FONT';xt:'.FNT'),
		(tag:'PLTT';xt:'.PLT'),
		(tag:'GMID';xt:'.GMD'),
		(tag:'VOIC';xt:'.VOC'),
		(tag:'GMID';xt:'.MID'),
		(tag:'DELT';xt:'.DEL'),
		(tag:'ANIM';xt:'.ANI'),
		(tag:'FONT';xt:'.FON'),
		(tag:'TEXT';xt:'.TXT'));
var i:integer;
begin
ext2tag:='';
for i:=1 to sizeof(table) div sizeof(entry) do
begin
 if e=table[i].xt then begin ext2tag:=table[i].tag; break; end;
end;
end;


procedure prepare(var s:string);
var i:integer;l:byte;
begin
 l:=pos('#',s);
 if l<>0 then s[0]:=chr(l-1);
 for i:=1 to length(s) do s[i]:=upcase(s[i]);
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

function getword(var p:string; pos:byte; var s:string):byte;
var i:integer;
begin
 i:=pos; s:=''; getword:=i;
 while (p[i] in [' ',#9]) and (i<=length(p)) do inc(i);
 if i>length(p) then exit;
 s:=p[i]; inc(i);
 while not (p[i] in [' ',#9,'/']) and (i<=length(p)) do begin s:=concat(s,p[i]); inc(i) end;
 getword:=i;
end;

procedure skip2next(var t:text);
begin
 repeat
  getline(t,s,false);
 until s='';
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
   '/': begin pm.sw:=pm.sw+w[2]; pm.swpos[length(pm.sw)]:=i-length(w); end;
   '-': if minus then begin pm.sw:=pm.sw+w[2]; pm.swpos[length(pm.sw)]:=i-length(w); end
    else begin if n<=3 then pm.files[n]:=w; inc(n); end;
  else
    begin
     if n<=3 then pm.files[n]:=w;
     inc(n);
    end;
  end;
 until (w='') or (i>length(s));
end;

function nt2s(var n;i:byte):string;
var s:string;
begin
 fillchar(s,sizeof(s),0);
 move(n,s[1],i);
 s[0]:=chr(i);
 if pos(#0,s)<>0 then s[0]:=chr(pos(#0,s)-1);
 nt2s:=s;
end;

type
	film_ident=string[20];
	cmd_desc=record cmd:integer;n1,n2:byte; name:string[10]; end;
	pcmd_desc=^cmd_desc;
	cmd_params=record
			n:byte;
			p:array[0..15] of integer;
		     end;
const
	bad_value=-32000;
	fc_data:array[0..27] of cmd_desc=
	((cmd:2;n1:0;n2:0;name:'END'),
	(cmd:3;n1:1;n2:1;name:'*TIME*'),
	(cmd:4;n1:4;n2:7;name:'MOVE'),
	(cmd:5;n1:4;n2:7;name:'SPEED'),
	(cmd:6;n1:1;n2:1;name:'LAYER'),
	(cmd:7;n1:2;n2:2;name:'FRAME'),
	(cmd:8;n1:2;n2:2;name:'ANIMATE'),
	(cmd:9;n1:1;n2:1;name:'CUE'),
	(cmd:10;n1:2;n2:2;name:'VAR'),
	(cmd:11;n1:4;n2:4;name:'WINDOW'),
	(cmd:13;n1:1;n2:1;name:'SWITCH'),
	(cmd:14;n1:2;n2:2;name:'????'),
	(cmd:15;n1:1;n2:1;name:'PALETTE'),
	(cmd:18;n1:2;n2:2;name:'CUT'),
	(cmd:20;n1:1;n2:1;name:'LOOP'),
	(cmd:24;n1:1;n2:1;name:'PRELOAD'),
	(cmd:25;n1:4;n2:4;name:'SOUND'),
	(cmd:28;n1:7;n2:7;name:'STEREO'),
	(cmd:3;n1:1;n2:1;name:'TIME'),
	(cmd:28;n1:7;n2:7;name:'!SOUND'),
	(cmd:4;n1:4;n2:7;name:'MOVETO'),
	(cmd:-1;n1:1;n2:1;name:'>END'),
	(cmd:-2;n1:1;n2:1;name:'>VIEW'),
	(cmd:-3;n1:1;n2:1;name:'>DELT'),
	(cmd:-3;n1:1;n2:1;name:'>ANIM'),
	(cmd:-3;n1:1;n2:1;name:'>CUST'),
	(cmd:-4;n1:1;n2:1;name:'>PLTT'),
	(cmd:-5;n1:1;n2:1;name:'>VOIC'));

function get_cmd_byid(cmd:integer):pcmd_desc;
var i:integer;
begin
 get_cmd_byid:=nil;
 for i:=0 to sizeof(fc_data) div sizeof(cmd_desc)-1 do
  if fc_data[i].cmd=cmd then begin get_cmd_byid:=@fc_data[i]; exit; end;
end;

Function get_cmd_byname(s:film_ident):pcmd_desc;
var i:integer;
begin
 get_cmd_byname:=nil;
 for i:=0 to sizeof(fc_data) div sizeof(cmd_desc)-1 do
  if fc_data[i].name=s then begin get_cmd_byname:=@fc_data[i]; exit; end;
end;

Function ConvertStr(s: film_ident;duration:integer): Integer;
function tentimes(s:film_ident):integer;
var i,j:integer; a,b:word; s1:string[20];p:byte;
begin
 p:=pos('.',s);
 s1:=copy(s,p+1,1);
 s[0]:=chr(p-1);
 val(s,i,a);
 val(s1,j,b);
 i:=i*10+j;
 if a+b<>0 then i:=bad_value;
 tentimes:=i;
end;

Var
i: Integer; a:word;
Begin
if s[length(s)]='%' then dec(s[0]);
val(s,i,a);
if a<>0 then
if s='NORMAL' then i:=0 else
if s='SPECIAL' then i:=128 else
if s='RANDOMX' then i:=Random(320) else
if s='RANDOMY' then i:=Random(200) else
if s='RANDOM' then i:=Random(256) else
if s='RANDOMSPEED' then i:=Random(7)-3 else
if s='STRANGE' then i:=64 else
if s='START' then i:=0 else
if s='FINAL' then i:=duration-1 else
if s='OFF' then i:=0 else
if s='ON' then i:=1 else
if s='FORWARDS' then i:=1 else
if s='BACKWARDS' then i:=-1 else
if s='SET' then i:=0 else
If s='END' then i:=3 else
If s='*TIME*' then i:=3 else
If s='TIME' then i:=3 else
If s='MOVE' then i:=4 else
if s='MOVETO' then i:=4 else
if s='SPEED' then i:=5 else
if s='LAYER' then i:=6 else
if s='FRAME' then i:=7 else
if s='ANIMATE' then i:=8 else
if s='CUE' then i:=9 else
if s='VAR' then i:=10 else
if s='SIZE' then i:=11 else
if s='[TWELVE]' then i:=12 else
if s='SWITCH' then i:=13 else
if s='????' then i:=14 else
if s='PALETTE' then i:=15 else
if s='[SIXTEEN]' then i:=16 else
if s='[SEVENTEEN]' then i:=17 else
if s='CUT' then i:=18 else
if s='[NINETEEN]' then i:=19 else
if s='LOOP' then i:=20 else
if s='[TWENTYONE]' then i:=21 else
if s='[TWENTYTWO]' then i:=22 else
if s='[TWENTYTHREE]' then i:=23 else
if s='PRELOAD' then i:=24 else
if s='SOUND' then i:=25 else
if s='[TWENTYSIX]' then i:=26 else
if s='[TWENTYSEVEN]' then i:=27 else
if s='!SOUND' then i:=28 else
if s='STEREO' then i:=28 else
if s='CENTER' then i:=127 else
if s='MIDDLE' then i:=128 else
if s='LEFT' then i:=0 else
if s='RIGHT' then i:=255 else
if s='ABOVE' then i:=-200 else
if s='BELOW' then i:=200 else
if s='DOUBLEABOVE' then i:=-400 else
if s='DOUBLEBELOW' then i:=400 else
if s='STAGELEFT' then i:=-320 else
if s='STAGERIGHT' then i:=320 else
if s='DOUBLELEFT' then i:=-640 else
if s='DOUBLERIGHT' then i:=640 else
if s='CENTERLEFT' then i:=63 else
if s='MIDDLELEFT' then i:=64 else
if s='CENTERRIGHT' then i:=191 else
if s='MIDDLERIGHT' then i:=192 else
if s='SWAP' then i:=1 else
if s='CLEAR' then i:=2 else
if s='DIRTY' then i:=3 else
if s='FADERIGHT' then i:=12 else
if s='FADELEFT' then i:=13 else
if s='FADEUP' then i:=14 else
if s='FADEDOWN' then i:=15 else
if s='FADEUPDOWN' then i:=21 else
if s='STOP' then i:=23 else
if s='FADETOBLACK' then i:=2333 else
if s='LOAD' then i:=1 else
if s='OLD' then i:=2 else
if s='FINISH' then i:=3 else
if s='NEW' then i:=4 else
if s='FILMLENGTH' then i:=duration else
if Pos('.',s)>0 then i:=TenTimes(s) else i:=bad_value;
ConvertStr:=i;
End;

Function get_cmd_params(var s:string;var p:cmd_params;duration:integer):integer;
var ps:byte; w:string;i:integer;
begin
 get_cmd_params:=-1;
 ps:=getword(s,1,w); p.n:=0;
 ps:=getword(s,ps,w);
 while (w<>''){ and (ps<=length(s))} do
 begin
  i:=convertstr(w,duration);
  if i=bad_value then begin get_cmd_params:=-1; exit; end;
  p.p[p.n]:=i;inc(p.n);
  ps:=getword(s,ps,w);
 end;
 get_cmd_params:=0;
end;