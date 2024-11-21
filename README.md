### **Defragmentation Problem in Cloud Computing**

---

#### **Mathematical Formulation:**

**Decision Variables:**

- $x_{ij} \in \{0, 1\}$: Indicates if Virtual Machine (VM) $i$ is assigned to Host $j$ after migration.
- $y_i \in \{0, 1\}$: Indicates if VM $i$ has been migrated.
- $z_j \in \{0, 1\}$: Indicates if Host $j$ is active after migration.

**Parameters:**

- $x_{ij}^0 \in \{0, 1\}$: Original assignment of VM $i$ to Host $j$.
- $P_i^{\text{CPU}}$: CPU demand of VM $i$.
- $P_j^{\text{CPU}}$: CPU capacity of Host $j$.
- $\Omega_k^{\text{Anti}}$: Sets of VMs that should not be co-located (anti-affinity groups).
- $\Omega^{\text{unMig}}$: Set of VMs that are not allowed to be migrated.
- $\Omega^{\text{notMig}}$: Set of hosts that are unavailable for new VM assignments.
- $b_i \in \{0,1\}$: Indicates if VM $i$ is allowed to be migrated ($b_i = 1$) or not ($b_i = 0$).
- $\alpha$: Weighting factor balancing the importance between minimizing active hosts and minimizing VM migrations.

**Objective Function:**

$$
\min \quad \alpha \sum_{j=1}^{N_{\text{HOST}}} z_j + \sum_{i=1}^{N_{\text{VM}}} y_i
$$

**Constraints:**

1. **VM Assignment Constraints:**

   Each VM must be assigned to exactly one host:

$$
   \sum_{j=1}^{N_{\text{HOST}}} x_{ij} = 1, \quad \forall i = 1, \dots, N_{\text{VM}}
$$

2. **Migration Indicator Constraints:**

   Determine if a VM has been migrated:

$$
   y_i \geq x_{ij}^0 - x_{ij}, \quad \forall i, \forall j
$$

$$
   y_i \geq x_{ij} - x_{ij}^0, \quad \forall i, \forall j
$$

3. **Host Activation Constraints:**

   A host is active if any VM is assigned to it:

$$
   z_j \geq x_{ij}, \quad \forall i, \forall j
$$

4. **Resource Capacity Constraints:**

   Total resource demands of VMs assigned to a host must not exceed its capacity:

$$
   \sum_{i=1}^{N_{\text{VM}}} P_i^{\text{CPU}} x_{ij} \leq P_j^{\text{CPU}} z_j, \quad \forall j
$$

5. **Anti-Affinity Constraints:**

   VMs in the same anti-affinity group cannot be placed on the same host:

$$
   \sum_{i \in \Omega_k^{\text{Anti}}} x_{ij} \leq 1, \quad \forall j, \forall k
$$

6. **Non-Migratable VMs Constraints:**

   VMs that cannot be migrated must remain on their original hosts:

$$
   y_i = 0, \quad \forall i \in \Omega^{\text{unMig}}
$$

$$
   x_{ij} = x_{ij}^0, \quad \forall i \in \Omega^{\text{unMig}}, \forall j
$$

7. **Host Exclusion Constraints:**

   VMs cannot be assigned to unavailable hosts:

$$
   x_{ij} = 0, \quad \forall i, \forall j \in \Omega^{\text{notMig}}
$$

8. **Migration Permission Constraints:**

   Only VMs allowed to migrate can be reassigned:

$$
   y_i \leq b_i, \quad \forall i
$$

9. **Variable Domains:**

$$
   x_{ij} \in \{0, 1\}, \quad y_i \in \{0, 1\}, \quad z_j \in \{0, 1\}
$$

---

### **Similar Problems with Mathematical Formulations**

---

#### **1. Bin Packing Problem**

**Objective:** Minimize the number of bins used.

**Variables:**

- $x_{ij} \in \{0, 1\}$: Indicates if Item $i$ is placed in Bin $j$.
- $z_j \in \{0, 1\}$: Indicates if Bin $j$ is used.

**Parameters:**

- $s_i$: Size of Item $i$.
- $C$: Capacity of each bin (assuming identical bins).

**Objective Function:**

$$
\min \quad \sum_{j=1}^{N_{\text{BIN}}} z_j
$$

**Constraints:**

1. **Item Assignment Constraints:**

   Each item must be assigned to exactly one bin:

$$
   \sum_{j=1}^{N_{\text{BIN}}} x_{ij} = 1, \quad \forall i
$$

2. **Bin Capacity Constraints:**

   The total size of items in a bin cannot exceed its capacity:

$$
   \sum_{i=1}^{N_{\text{ITEM}}} s_i x_{ij} \leq C z_j, \quad \forall j
$$

3. **Bin Activation Constraints:**

   A bin is used if any item is placed in it:

$$
   x_{ij} \leq z_j, \quad \forall i, \forall j
$$

4. **Variable Domains:**

$$
   x_{ij} \in \{0, 1\}, \quad z_j \in \{0, 1\}
$$

**Parallel to Defragmentation Problem:**

- **Items ↔ VMs**
- **Bins ↔ Hosts**
- **Item Sizes ↔ VM Resource Demands**
- **Bin Capacities ↔ Host Capacities**
- **Objective:** Minimize the number of bins (active hosts).

---

#### **2. Task Scheduling in Distributed Systems**

**Objective:** Minimize the number of active processors and task migrations.

**Variables:**

- $x_{ij} \in \{0, 1\}$: Indicates if Task $i$ is assigned to Processor $j$.
- $y_i \in \{0, 1\}$: Indicates if Task $i$ has been migrated.
- $z_j \in \{0, 1\}$: Indicates if Processor $j$ is active.

**Parameters:**

- $x_{ij}^0$: Original assignment of Task $i$ to Processor $j$.
- $c_i$: Computational demand of Task $i$.
- $C_j$: Computational capacity of Processor $j$.
- $\alpha$: Weighting factor.

**Objective Function:**

$$
\min \quad \alpha \sum_{j} z_j + \sum_{i} y_i
$$

**Constraints:**

1. **Task Assignment Constraints:**

   Each task must be assigned to exactly one processor:

$$
   \sum_{j} x_{ij} = 1, \quad \forall i
$$

2. **Processor Capacity Constraints:**

   Total computational demands on a processor must not exceed its capacity:

$$
   \sum_{i} c_i x_{ij} \leq C_j z_j, \quad \forall j
$$

3. **Processor Activation Constraints:**

   A processor is active if any task is assigned to it:

$$
   x_{ij} \leq z_j, \quad \forall i, \forall j
$$

4. **Migration Indicator Constraints:**

   Determine if a task has been migrated:

$$
   y_i \geq x_{ij}^0 - x_{ij}, \quad \forall i, \forall j
$$

$$
   y_i \geq x_{ij} - x_{ij}^0, \quad \forall i, \forall j
$$

5. **Variable Domains:**

$$
   x_{ij}, y_i, z_j \in \{0, 1\}
$$

**Parallel to Defragmentation Problem:**

- **Tasks ↔ VMs**
- **Processors ↔ Hosts**
- **Computational Demands ↔ VM Resource Demands**
- **Objective:** Minimize active processors (hosts) and task (VM) migrations.

---

#### **3. Network Traffic Engineering**

**Objective:** Minimize the number of active network links and flow reroutings.

**Variables:**

- $x_{ij}^k \in \{0, 1\}$: Indicates if Flow $k$ uses Link $(i, j)$.
- $y_k \in \{0, 1\}$: Indicates if Flow $k$ has been rerouted.
- $z_{ij} \in \{0, 1\}$: Indicates if Link $(i, j)$ is active.

**Parameters:**

- $x_{ij}^{k,0}$: Original routing of Flow $k$ on Link $(i, j)$.
- $b_k$: Bandwidth requirement of Flow $k$.
- $C_{ij}$: Capacity of Link $(i, j)$.
- $\alpha$: Weighting factor.

**Objective Function:**

$$
\min \quad \alpha \sum_{(i,j)} z_{ij} + \sum_{k} y_k
$$

**Constraints:**

1. **Flow Conservation Constraints:**

   Ensure flows are properly routed from source to destination.

   (Detailed flow conservation equations involving $x_{ij}^k$ at each node.)

2. **Link Capacity Constraints:**

   Total bandwidth on a link cannot exceed its capacity:

$$
   \sum_{k} b_k x_{ij}^k \leq C_{ij} z_{ij}, \quad \forall (i, j)
$$

3. **Link Activation Constraints:**

   A link is active if any flow uses it:

$$
   x_{ij}^k \leq z_{ij}, \quad \forall k, \forall (i, j)
$$

4. **Flow Rerouting Indicator Constraints:**

   Determine if a flow has been rerouted:

$$
   y_k \geq x_{ij}^{k,0} - x_{ij}^k, \quad \forall k, \forall (i, j)
$$

$$
   y_k \geq x_{ij}^k - x_{ij}^{k,0}, \quad \forall k, \forall (i, j)
$$

5. **Variable Domains:**

$$
   x_{ij}^k, y_k, z_{ij} \in \{0, 1\}
$$

**Parallel to Defragmentation Problem:**

- **Flows ↔ VMs**
- **Network Links ↔ Hosts**
- **Bandwidth Requirements ↔ VM Resource Demands**
- **Objective:** Minimize active links (hosts) and flow (VM) reroutings (migrations).

---

#### **4. Data Replication in Distributed Databases**

**Objective:** Minimize the number of active servers and data replication costs.

**Variables:**

- $x_{ij} \in \{0, 1\}$: Indicates if Data Item $i$ is stored on Server $j$.
- $y_i \in \{0, 1\}$: Indicates if Data Item $i$ has been replicated.
- $z_j \in \{0, 1\}$: Indicates if Server $j$ is active.

**Parameters:**

- $x_{ij}^0$: Original storage of Data Item $i$ on Server $j$.
- $s_i$: Size of Data Item $i$.
- $C_j$: Storage capacity of Server $j$.
- $R_i$: Required number of replicas for Data Item $i$.
- $\alpha$: Weighting factor.

**Objective Function:**

$$
\min \quad \alpha \sum_{j} z_j + \sum_{i} y_i
$$

**Constraints:**

1. **Replication Constraints:**

   Each data item must have the required number of replicas:

$$
   \sum_{j} x_{ij} = R_i, \quad \forall i
$$

2. **Server Capacity Constraints:**

   Total size of data items on a server must not exceed its capacity:

$$
   \sum_{i} s_i x_{ij} \leq C_j z_j, \quad \forall j
$$

3. **Server Activation Constraints:**

   A server is active if any data item is stored on it:

$$
   x_{ij} \leq z_j, \quad \forall i, \forall j
$$

4. **Replication Indicator Constraints:**

   Determine if data item has been replicated:

$$
   y_i \geq \sum_{j} (x_{ij} - x_{ij}^0), \quad \forall i
$$

5. **Variable Domains:**

$$
   x_{ij}, y_i, z_j \in \{0, 1\}
$$

**Parallel to Defragmentation Problem:**

- **Data Items ↔ VMs**
- **Servers ↔ Hosts**
- **Data Sizes ↔ VM Resource Demands**
- **Objective:** Minimize active servers (hosts) and replication operations (VM migrations).

---

#### **5. Cloud Service Placement in Edge Computing**

**Objective:** Minimize the number of active edge servers and service migrations.

**Variables:**

- $x_{ij} \in \{0, 1\}$: Indicates if Service $i$ is placed on Edge Server $j$.
- $y_i \in \{0, 1\}$: Indicates if Service $i$ has been migrated.
- $z_j \in \{0, 1\}$: Indicates if Edge Server $j$ is active.

**Parameters:**

- $x_{ij}^0$: Original placement of Service $i$ on Edge Server $j$.
- $r_i$: Resource requirement of Service $i$.
- $C_j$: Capacity of Edge Server $j$.
- $\alpha$: Weighting factor.

**Objective Function:**

$$
\min \quad \alpha \sum_{j} z_j + \sum_{i} y_i
$$

**Constraints:**

1. **Service Assignment Constraints:**

   Each service must be assigned to exactly one edge server:

$$
   \sum_{j} x_{ij} = 1, \quad \forall i
$$

2. **Edge Server Capacity Constraints:**

   Total resource demands on an edge server must not exceed its capacity:

$$
   \sum_{i} r_i x_{ij} \leq C_j z_j, \quad \forall j
$$

3. **Edge Server Activation Constraints:**

   An edge server is active if any service is assigned to it:

$$
   x_{ij} \leq z_j, \quad \forall i, \forall j
$$

4. **Migration Indicator Constraints:**

   Determine if a service has been migrated:

$$
   y_i \geq x_{ij}^0 - x_{ij}, \quad \forall i, \forall j
$$

$$
   y_i \geq x_{ij} - x_{ij}^0, \quad \forall i, \forall j
$$

5. **Variable Domains:**

$$
   x_{ij}, y_i, z_j \in \{0, 1\}
$$

**Parallel to Defragmentation Problem:**

- **Services ↔ VMs**
- **Edge Servers ↔ Hosts**
- **Resource Requirements ↔ VM Demands**
- **Objective:** Minimize active edge servers (hosts) and service (VM) migrations.

---

#### **6. Load Balancing in Content Delivery Networks (CDNs)**

**Objective:** Minimize the number of active CDN servers and content movements.

**Variables:**

- $x_{ij} \in \{0, 1\}$: Indicates if Content $i$ is stored on Server $j$.
- $y_i \in \{0, 1\}$: Indicates if Content $i$ has been moved.
- $z_j \in \{0, 1\}$: Indicates if Server $j$ is active.

**Parameters:**

- $x_{ij}^0$: Original storage of Content $i$ on Server $j$.
- $s_i$: Size of Content $i$.
- $C_j$: Storage capacity of Server $j$.
- $R_i$: Required number of replicas for Content $i$.
- $\alpha$: Weighting factor.

**Objective Function:**

$$
\min \quad \alpha \sum_{j} z_j + \sum_{i} y_i
$$

**Constraints:**

1. **Content Placement Constraints:**

   Each content item must be stored on the required number of servers:

$$
   \sum_{j} x_{ij} = R_i, \quad \forall i
$$

2. **Server Capacity Constraints:**

   Total size of content on a server must not exceed its capacity:

$$
   \sum_{i} s_i x_{ij} \leq C_j z_j, \quad \forall j
$$

3. **Server Activation Constraints:**

   A server is active if any content is stored on it:

$$
   x_{ij} \leq z_j, \quad \forall i, \forall j
$$

4. **Content Movement Indicator Constraints:**

   Determine if content has been moved:

$$
   y_i \geq \sum_{j} (x_{ij} - x_{ij}^0), \quad \forall i
$$

5. **Variable Domains:**

$$
   x_{ij}, y_i, z_j \in \{0, 1\}
$$

**Parallel to Defragmentation Problem:**

- **Content Items ↔ VMs**
- **Servers ↔ Hosts**
- **Content Sizes ↔ VM Resource Demands**
- **Objective:** Minimize active servers (hosts) and content movements (VM migrations).

---

#### **7. Multi-Commodity Flow Problem**

**Objective:** Minimize the number of active arcs and flow costs.

**Variables:**

- $x_{ij}^k \geq 0$: Flow of Commodity $k$ on Arc $(i, j)$.
- $z_{ij} \in \{0, 1\}$: Indicates if Arc $(i, j)$ is active.

**Parameters:**

- $d_k$: Demand of Commodity $k$.
- $C_{ij}$: Capacity of Arc $(i, j)$.
- $c_{ij}^k$: Cost per unit flow of Commodity $k$ on Arc $(i, j)$.
- $\alpha$: Weighting factor.

**Objective Function:**

$$
\min \quad \alpha \sum_{(i,j)} z_{ij} + \sum_{(i,j)} \sum_{k} c_{ij}^k x_{ij}^k
$$

**Constraints:**

1. **Flow Conservation Constraints:**

   For each node $v$ and commodity $k$:

$$
   \sum_{i} x_{iv}^k - \sum_{j} x_{vj}^k = \begin{cases}
   d_k, & \text{if } v = \text{source of } k \\
   -d_k, & \text{if } v = \text{destination of } k \\
   0, & \text{otherwise}
   \end{cases}
$$

2. **Arc Capacity Constraints:**

   Total flow on an arc cannot exceed its capacity:

$$
   \sum_{k} x_{ij}^k \leq C_{ij} z_{ij}, \quad \forall (i, j)
$$

3. **Arc Activation Constraints:**

   An arc is active if any commodity flows through it:

$$
   x_{ij}^k \leq C_{ij} z_{ij}, \quad \forall k, \forall (i, j)
$$

4. **Variable Domains:**

$$
   x_{ij}^k \geq 0, \quad z_{ij} \in \{0, 1\}
$$

**Parallel to Defragmentation Problem:**

- **Commodities ↔ VMs**
- **Arcs ↔ Hosts**
- **Arc Capacities ↔ Host Capacities**
- **Objective:** Minimize active arcs (hosts) and flow costs (migration costs).

---

### **Summary**

In each of these problems, the mathematical formulations involve:

- **Assignment Variables ($x$)**: Indicating the allocation of items, tasks, flows, services, or content to resources.
- **Activation Variables ($z$)**: Indicating whether a resource is active based on assignments.
- **Migration or Movement Indicators ($y$)**: Reflecting whether items have been moved, migrated, or rerouted.
- **Objective Functions**: Aimed at minimizing the number of active resources and the costs associated with reallocations or migrations.
- **Constraints**: Ensuring feasible assignments without exceeding resource capacities and adhering to specific requirements or rules.
- **Variables Domains**: Binary variables for assignments and activations, non-negative variables for flow quantities.

By mapping the defragmentation problem's mathematical structure to these problems, we can apply similar optimization techniques across different domains, facilitating efficient solutions to large-scale resource allocation challenges.
