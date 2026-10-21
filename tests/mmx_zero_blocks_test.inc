/* Arrange contact with real Flame Mammoth terrain, then execute the original
 * destruction routines. Inventory and destroyed tiles remain guest-owned. */
static unsigned blocks_cell(unsigned x,unsigned y) {
  unsigned screen=g_ram[0xe800+(y>>8)*32+(x>>8)];
  return 0x2000+((screen*512+((y&240)<<1)+((x&240)>>3))&65535);
}
static void zero_blocks_checks(const char *assets) {
  check(assets && MmxZeroLoad(assets),"block test loads X3 Zero");
  MmxZeroRegisterHooks();
  hitbox_boot();
  g_ram[0xd1]=2;g_ram[0xd2]=2;g_ram[0xd3]=g_ram[0xd4]=0;
  g_ram[0x1f7a]=4;g_ram[0x1f81]=g_ram[0x1f82]=0;g_ram[0x1f99]=0;
  for(unsigned i=0;i<1800;++i) {
    frame(0);
    if(g_ram[0xd3]==4 && g_ram[0xba9]==2) break;
  }
  check(g_ram[0x1f7a]==4 && g_ram[0xd3]==4 && g_ram[0xba9]==2,
      "block test enters Flame Mammoth without armor");
  unsigned bx=0,by=0;
  const uint8_t *rom=g_snes->cart->rom;
  for(unsigned y=16;y<2048 && !bx;y+=16) for(unsigned x=16;x<8192;x+=16) {
    unsigned cell=blocks_cell(x,y);
    unsigned address=eagle_word(g_ram+0xb92)+(g_ram[0xb94]<<16)+eagle_word(g_ram+cell);
    size_t offset=((address>>16)&127)*0x8000+(address&0x7fff);
    if((address&65535)>=0x8000 && offset<(size_t)g_snes->cart->romSize &&
        (rom[offset]&63)==0x3c) {bx=x;by=y;break;}
  }
  check(bx!=0,"real stage contains a helmet-breakable block");
  printf("BLOCK fixture x=%u y=%u cell=%04x\n",bx,by,blocks_cell(bx,by));
  static uint8_t world[0x20000];memcpy(world,g_ram,sizeof(world));
  for(unsigned interpreted=0;interpreted<2;++interpreted)
      for(unsigned foot=0;foot<2;++foot) for(unsigned modern=0;modern<2;++modern)
      for(unsigned x=0;x<2;++x) for(unsigned owned=0;owned<2;++owned) {
    memcpy(g_ram,world,sizeof(world));
    MmxZeroSetModern(modern);MmxZeroResetState();
    MmxZeroState z=MmxZeroGetState();z.active_x=x;MmxZeroSelectState(z);
    g_ram[0x1f99]=owned?(foot?8:1):0;
    eagle_putword(g_ram+0xbad,bx+8);eagle_putword(g_ram+0xbb0,by+(foot?-8:19));
    g_ram[0xbd1]=0;
    unsigned cell=blocks_cell(bx,by),before=eagle_word(g_ram+cell),inventory=g_ram[0x1f99];
    CpuState cpu=g_cpu;cpu.PB=0x81;cpu.DB=0;cpu.D=0xba8;cpu.S=0x1e00;cpu.P=0x30;
    cpu_p_to_mirrors(&cpu);
    unsigned pc=foot?0x8198fc:0x819ea1;
    if(interpreted) {
      /* RTS sentinel for the bridge's balanced return past its entry. */
      cpu_write8(&cpu,0,cpu.S--,0x80);cpu_write8(&cpu,0,cpu.S--,0);
      check(interp_bridge_run(&cpu,pc),"interpreted block routine returns");
    } else {
      check(cpu_dispatch_call_pc(&cpu,pc,0x818000)==RECOMP_RETURN_NORMAL,
          "compiled block routine returns");
    }
    bool changed=eagle_word(g_ram+cell)!=before;
    printf("BLOCK interpreted=%u foot=%u modern=%u x=%u owned=%u tile=%04x->%04x\n",
        interpreted,foot,modern,x,owned,before,eagle_word(g_ram+cell));
    check(changed==(!x || owned),"Zero crushes by default; X requires the matching upgrade");
    check(g_ram[0x1f99]==inventory,"crushing leaves actual upgrade ownership unchanged");
  }
  MmxZeroSetModern(false);MmxZeroResetState();
  puts("MMX ZERO NATIVE BLOCK CHECKS PASSED");
}
