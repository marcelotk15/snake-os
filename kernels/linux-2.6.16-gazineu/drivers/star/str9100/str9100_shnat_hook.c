/******************************************************************************
 *
 *  Copyright (c) 2008 Cavium Networks 
 * 
 *  This file is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License, Version 2, as 
 *  published by the Free Software Foundation. 
 *
 *  This file is distributed in the hope that it will be useful, 
 *  but AS-IS and WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE, TITLE, or 
 *  NONINFRINGEMENT.  See the GNU General Public License for more details. 
 *
 *  You should have received a copy of the GNU General Public License 
 *  along with this file; if not, write to the Free Software 
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA or 
 *  visit http://www.gnu.org/licenses/. 
 *
 *  This file may also be available under a different license from Cavium. 
 *  Contact Cavium Networks for more information
 *
 ******************************************************************************/


#include <linux/config.h>

#ifdef CONFIG_NETFILTER
#include <linux/types.h>
#include <linux/inetdevice.h>
#include <linux/ip.h>
#include <linux/timer.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <net/protocol.h>
#include <net/ip.h>
#include <net/checksum.h>
#include <net/route.h>
#include <linux/netfilter_ipv4.h>
#include <linux/netfilter_ipv4/ip_nat_rule.h>
#include <linux/netfilter_ipv4/ip_tables.h>

#include <linux/str9100/star9100_shnat.h>
#include <linux/str9100/str9100_shnat_hook.h>

//#ifdef CONFIG_STAR9100_SHNAT_PCI_FASTPATH
struct net_device *pci_netdev[MAX_FP_PCIDEV];
EXPORT_SYMBOL(pci_netdev);
int (*star9100_shnat_pci_fp_forward_skb_ptr)(struct sk_buff *skb);
//#endif

int star9100_shnat_hook_ready;
int (*star9100_shnat_preadd_hnatable_hook)(u32 sip,u16 sport,u16 dport, u32 proto);
int (*star9100_shnat_check_shnat_enable_hook)(void);
int (*star9100_shnat_nf_nat_preadd_hnatable_hook)(const struct ip_conntrack *ct, int dir, const u16 port);
int (*star9100_shnat_nf_remove_hnatable_hook)(struct ip_conntrack *);
int (*star9100_shnat_nf_add_hnatable_hook)(const struct ip_conntrack *ct,const struct iphdr *iph, u16 proto);
int (*star9100_shnat_add_arptable_hook)(u32 myip, u32 targetip);
int (*star9100_shnat_fix_arptable_hook)(u32 myip, u32 targetip);
int (*star9100_shnat_nf_preadd_hnatable_hook)(const struct sk_buff **pskb);
int (*star9100_shnat_check_ftponly_enable_hook)(void);
int (*star9100_shnat_pci_fp_getdev_hook)(struct sk_buff *skb_ptr);
star9100_arp_table *(*star9100_shnat_getarptable_hook)( u32 ip_addr);
void (*shnat_api_hook)(shnat_input *input, shnat_return *ret);


EXPORT_SYMBOL(star9100_shnat_hook_ready);
EXPORT_SYMBOL(star9100_shnat_preadd_hnatable_hook);
EXPORT_SYMBOL(star9100_shnat_check_shnat_enable_hook);
EXPORT_SYMBOL(star9100_shnat_nf_nat_preadd_hnatable_hook);
EXPORT_SYMBOL(star9100_shnat_nf_remove_hnatable_hook);
EXPORT_SYMBOL(star9100_shnat_nf_add_hnatable_hook);
EXPORT_SYMBOL(star9100_shnat_add_arptable_hook);
EXPORT_SYMBOL(star9100_shnat_fix_arptable_hook);
EXPORT_SYMBOL(star9100_shnat_nf_preadd_hnatable_hook);
EXPORT_SYMBOL(star9100_shnat_check_ftponly_enable_hook);
EXPORT_SYMBOL(star9100_shnat_getarptable_hook);
EXPORT_SYMBOL(star9100_shnat_pci_fp_getdev_hook);
EXPORT_SYMBOL(star9100_shnat_pci_fp_forward_skb_ptr);
EXPORT_SYMBOL(shnat_api_hook);
#endif
