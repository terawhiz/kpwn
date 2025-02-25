#define map_lookup_elem 1
#define aliyunctf_xor 212
#define skb_load_bytes 68

#define LOL 0x133713371337
#define NOP r2 = LOL

#define ldmap(reg, map) \
	ld_pseudo reg, 1, map

program:
	NOP
	NOP
	r9 = r1

	r2 = r10
	r2 += -8
	*(u64 *)(r2 + 0) = 0
	ldmap(r1, 3)
	call map_lookup_elem
	if r0 == 0 goto err

	r7 = r0

	r8 = r10
	r8 += -8
	*(u64 *)(r8+0) = 0x45 ^ 2025 // payload size ^ key

	r3 = r0 // res*, map elem addr here
	r2 = 8  // len
	r1 = r8 // buf*
	call aliyunctf_xor
	if r0 != 0 goto err

	r5 = 1
	r4 = *(u64*)(r7+0)	// verifier thinks r4 is 0
	r4 += 8
	r3 = r10
	r3 += -10
	r2 = 0
	r1 = r9
	call skb_load_bytes     // stack overflowwwwwwwwwwww

	r0 = 0
	exit

err:
	r0 = 1
	exit

